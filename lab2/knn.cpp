// C++ program to find groups of unknown
// Points using K nearest neighbour algorithm.
#include <bits/stdc++.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <random>
#include <cstdlib>

using namespace std;
#define ELEMENTS_SIZE 5000000
int NUM_THREADS = 2;

struct __attribute__((aligned(64))) Point
{
	int val;	 // Group of point
	double x;
	char padding1[48]; 
	double y;	 // Co-ordinate of point
	char padding2[48]; 
	double distance; // Distance from test point
};


Point * neighbours;

// Used to sort an array of points by increasing
// order of distance
// bool comparison(Point a, Point b)
// {
// 	return (a.distance < b.distance);
// }
int comparison(const void* a, const void* b, void* user_data)
{
    // Cast the parameters to the appropriate types
    const Point* point_a = static_cast<const Point*>(a);
    const Point* point_b = static_cast<const Point*>(b);
    
    // Your comparison logic here
    // For example, comparing distance:
    return (point_a->distance < point_b->distance) ? -1 : (point_a->distance > point_b->distance) ? 1 : 0;
}

// This function finds classification of point p using
// k nearest neighbour algorithm. It assumes only two
// groups and returns 0 if p belongs to group 0, else
// 1 (belongs to group 1).
int classifyAPoint(Point arr[], int n, int k, Point p)
{
	// Fill distances of all points from p
	for (int i = 0; i < n; i++)
		arr[i].distance = sqrt((arr[i].x - p.x) * (arr[i].x - p.x) + (arr[i].y - p.y) * (arr[i].y - p.y));

	// Sorting algorithm can't be parallize but use merge sort for best performance O(nlog(n))
	// sort(arr, arr+n, comparison);
	qsort_r(arr, n, sizeof(Point), comparison, nullptr);	//thread safe sorting algorithm

	// Now consider the first k elements and only
	// two groups
	int freq1 = 0;	 // Frequency of group 0
	int freq2 = 0;	 // Frequency of group 1
	for (int i = 0; i < k; i++)
	{
		if (arr[i].val == 0)
			freq1++;
		else if (arr[i].val == 1)
			freq2++;
	}

	return (freq1 > freq2 ? 0 : 1);
}


int classifyAPoint_parallel(Point arr[], int n, int k, Point p)
{	
	int freq1 = 0;	 // Frequency of group 0
	int freq2 = 0;	 // Frequency of group 1
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel
	{
		// Fill distances of all points from p
		#pragma omp for
		for (int i = 0; i < n; i++)
			arr[i].distance = sqrt((arr[i].x - p.x) * (arr[i].x - p.x) + (arr[i].y - p.y) * (arr[i].y - p.y));

		// Sorting algorithm can't be parallize but use merge sort for best performance O(nlog(n))
		// #pragma omp single
		// sort(arr, arr+n, comparison);
		qsort_r(arr, n, sizeof(Point), comparison, nullptr);

		// Now consider the first k elements and only
		// two groups
		#pragma omp for reduction(+: freq1) reduction(+: freq2)
		for (int i = 0; i < k; i++)
		{
			if (arr[i].val == 0)
				freq1++;
			else if (arr[i].val == 1)
				freq2++;
		}
	}

	return (freq1 > freq2 ? 0 : 1);
}

void initializeDataPoints_sequential(){
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Create uniform distribution for integers in the range [0, 10]
    std::uniform_int_distribution<int> distribution(0, 10);
    
    // Generate random x and y coordinates for each neighbour
    for(int i = 0; i < ELEMENTS_SIZE; i++){
        neighbours[i].x = distribution(gen);
        neighbours[i].y = distribution(gen);
        
        // Determine the value based on x and y coordinates
        neighbours[i].val = (neighbours[i].x > 5 && neighbours[i].y > 5) ? 1 : 0;
    }
}

void initializeDataPoints_parallel(){
	std::random_device rd;
	std::mt19937 gen(rd());

	omp_set_num_threads(NUM_THREADS);

	#pragma omp parallel for
	for(int i = 0; i < ELEMENTS_SIZE; i++) {
		// Create uniform distribution for integers in the range [0, 10]
		std::uniform_int_distribution<int> distribution(0, 10);
		
		// Generate random x and y coordinates
		neighbours[i].x = distribution(gen);
		neighbours[i].y = distribution(gen);

		// Determine the value based on x and y coordinates
		neighbours[i].val = (neighbours[i].x > 5 && neighbours[i].y > 5) ? 1 : 0;
	}
}

// Driver code
int main()
{
	printf("Numb threads running: %d\n", NUM_THREADS);
	//setting the number of threads for openMP
	omp_set_num_threads(NUM_THREADS);
	//initialize array dynamically
	neighbours = new Point[ELEMENTS_SIZE];

	//genrate the random numbers and ground them based on the property - sequential
	struct timespec begin, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &begin);
	initializeDataPoints_sequential();
	clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

	printf("Total Elapsed time - sequential_initialization: %f \n", elapsed);

	elapsed = 0;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	initializeDataPoints_parallel();
	clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    printf("Total Elapsed time - parallel_initialization: %f \n", elapsed);

	/*Testing Point*/
	Point p;
	p.x = 6.5;
	p.y = 5.6;

	// Parameter to decide group of the testing point
	int k = ELEMENTS_SIZE;

	elapsed = 0;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	printf ("The value classified to unknown point"
			" is %d.\n", classifyAPoint(neighbours, ELEMENTS_SIZE, k, p));
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    printf("Total Elapsed time  - sequntial_regression: %f \n", elapsed);


	elapsed = 0;
	clock_gettime(CLOCK_MONOTONIC, &begin);
	printf ("The value classified to unknown point"
			" is %d.\n", classifyAPoint_parallel(neighbours, ELEMENTS_SIZE, k, p));
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    printf("Total Elapsed time  - parallel_regression: %f \n", elapsed);

	free(neighbours);
	return 0;
}
