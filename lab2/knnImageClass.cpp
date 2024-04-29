#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <bits/stdc++.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <random>
#include <cstdlib>

using namespace std;

#pragma pack(push, 1)
typedef struct {
    char signature[2];
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int dataOffset;
} BMPHeader;

typedef struct {
    unsigned int headerSize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int colorsImportant;
} BMPInfoHeader;
#pragma pack(pop)

/**
 * @brief
 * contains the rgb value of each pixel
*/
struct ImagePixel{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

//one dimentional array for performance boost
ImagePixel * imageData = NULL;
ImagePixel * imageDataPointers[10][3]; //30 3 images for all 10 digits
BMPHeader header[10][3];
BMPInfoHeader infoHeader[10][3];

/**
 * @brief
 * to free all the allocated memory for each digit pixels
*/
void freePixelMem(){
    for(int i=0; i<10; i++){
        for(int j=0;j<3;j++){
            free(imageDataPointers[i][j]);
        }
    }
}

struct digitDistance{
    double distance;    //to keep track of the distance of the image compared to provided image
    int digit;
};

// Comparison function to sort digitDistance structs in ascending order based on the distance
int comparison(const void* a, const void* b, void* user_data){
    // Cast pointers to digitDistance structs
    const digitDistance* digitA = static_cast<const digitDistance*>(a);
    const digitDistance* digitB = static_cast<const digitDistance*>(b);

    // Compare distances
    if (digitA->distance < digitB->distance)
        return -1;
    else if (digitA->distance > digitB->distance)
        return 1;
    else
        return 0;
}


/**
 * @brief
 * implementing the knn algorithm on the image,
 * - find each pixel difference for the test image to provided image
 * - calcaulate this for all 10 images
 * - sort the result based on the relative distance value
 * - for the first k elements find frequency of the most occured image and that should be our result
*/
int classifyImage(BMPInfoHeader infoHeaderTestImage, ImagePixel testImage[], int k){
    digitDistance dist[30]; //since each digit has 3 corresponding images
    double tempDiff = 0;
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 3; j++){
            for(int t =0;t<(infoHeader[i][j].width * infoHeader[i][j].height);t++){
                tempDiff += sqrt(abs((imageDataPointers[i][j][t].blue - testImage[t].blue) * (imageDataPointers[i][j][t].blue - testImage[t].blue) + 
                (imageDataPointers[i][j][t].red - testImage[t].red) * (imageDataPointers[i][j][t].red - testImage[t].red) + 
                (imageDataPointers[i][j][t].green - testImage[t].green) * (imageDataPointers[i][j][t].green - testImage[t].green)));
            }
            tempDiff = tempDiff/(infoHeader[i][j].width * infoHeader[i][j].height);
            dist[i * 3 + j].distance = tempDiff; //setting this average diff to distance
            dist[i * 3 + j].digit = i;

            tempDiff = 0;
        }
    }

    qsort_r(dist, 30, sizeof(digitDistance), comparison, nullptr);
    int freq[10] = {0};

    for(int i=0; i<k; i++){ //update frequency of each number for the first k element
        freq[dist[i].digit]++;
    }

    int maxFreq = 0;
    int currIndex = 0;
    for(int i=0; i<10;i++){
        if(freq[i] > maxFreq){
            maxFreq = freq[i];
            currIndex = i;
        }
    }

    return currIndex;
}


int main() {
    FILE *file = NULL;

    for(int i=0; i<10;i++){
        for(int j = 0; j < 3; j++){

            string filePath = "./digits/image" + to_string(i) + to_string(j)  + ".bmp";
            // Open the BMP file
            file = fopen(filePath.c_str(), "rb");
            if (!file) {
                perror("Unable to open file");
                return 1;
            }

            // Read the BMP header
            fread(&header[i][j], sizeof(BMPHeader), 1, file);

            // Read the BMP info header
            fread(&infoHeader[i][j], sizeof(BMPInfoHeader), 1, file);

            //allocating the memory for imageData
            imageDataPointers[i][j] = (ImagePixel *)malloc(infoHeader[i][j].width * infoHeader[i][j].height * sizeof(ImagePixel));

            fseek(file,header[i][j].dataOffset,SEEK_SET); //offset it to the pixel data
            //read the content of the file in dynamic array
            for(int t =0;t<(infoHeader[i][j].width * infoHeader[i][j].height);t++){
                fread(&imageDataPointers[i][j][t],sizeof(imageDataPointers[i][j][t]),1,file);
            }

            // Close the training files
            fclose(file);
        }
    }

    //open the comparison files for testing
    file = fopen("testImage/6/74.bmp", "rb");
    if (!file) {
        perror("Unable to open file");
        return 1;
    }

    BMPHeader testHeader;
    BMPInfoHeader testInfoheader;
    // Read the BMP header
    fread(&testHeader, sizeof(BMPHeader), 1, file);

    // Read the BMP info header
    fread(&testInfoheader, sizeof(BMPInfoHeader), 1, file);
    imageData = (ImagePixel *)malloc(testInfoheader.width * testInfoheader.height * sizeof(ImagePixel));

    fseek(file,testHeader.dataOffset,SEEK_SET); //offset it to the pixel data
    //read the content of the file in dynamic array
    for(int t =0;t<(testInfoheader.width * testInfoheader.height);t++){
        fread(&imageData[t],sizeof(imageData[t]),1,file);
    }

    int k = 2;

    int predictedNumber = classifyImage(testInfoheader, imageData, k);

    printf("Predicted Number is: %d \n", predictedNumber);

    freePixelMem();
    free(imageData);
    return 0;
}
