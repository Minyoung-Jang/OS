#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define DESCENDING 0
#define DENSITY 1

typedef struct{
    int index;
    int weight;
    float benefit;
    float density;
} Element;

typedef struct{
    int w;
    int b;
    int index;
    float bound;
} Node;

int size = 0;

void merge(Element *ptr, int low, int mid, int high, int type)
{
    int mergedSize = high - low + 1;
    Element *temp = (Element *)malloc(mergedSize * sizeof(Element));
    int pos = 0;
    int left = low;
    int right = mid + 1;

    while (left <= mid && right <= high){
        if(type == 0){
            if (ptr[left].weight < ptr[right].weight){
                temp[pos++] = ptr[left++];
            }else{
                temp[pos++] = ptr[right++];
            }
        }else{
            if (ptr[left].density > ptr[right].density){
                temp[pos++] = ptr[left++];
            }else{
                temp[pos++] = ptr[right++];
            }
        }
    }

    while (left <= mid){
        temp[pos++] = ptr[left++];
    }

    while (right <= high){
        temp[pos++] = ptr[right++];
    }

    for (pos = 0; pos < mergedSize; ++pos)
        ptr[low + pos] = temp[pos];

    free(temp);
}

void merge_sort(Element *ptr, int low, int high, int type)
{
    if (low < high){
        int mid = (low + high) / 2;
        merge_sort(ptr, low, mid, type);
        merge_sort(ptr, mid + 1, high, type);
        merge(ptr, low, mid, high, type);
    }
}

void random_number_generator(Element *ptr, int items_No)
{
    srand(time(NULL));
    for(int i = 0; i < items_No; i++)
    {
        ptr[i].index = i+1;
        ptr[i].benefit = rand() % 300 + 1;
        ptr[i].weight = rand() % 100 + 1;
        ptr[i].density = (float)(ptr[i].benefit)/(float)(ptr[i].weight);
    }
}

void greedy(Element *ptr, int item_No, int total_weight, FILE *fp){
    int current_weight = 0;
    double benefit = 0;
    time_t start, end;
    double time;

    start = clock();

    for (int i = 0; i < item_No; i++) { 
		if (current_weight + ptr[i].weight <= total_weight) { 
			current_weight += ptr[i].weight; 
			benefit += ptr[i].benefit;
            //printf("Selected items : %d\n", ptr[i].index);
		}else{ 
			int remain = total_weight - current_weight; 
			benefit += ptr[i].benefit * ((double)remain / (double)ptr[i].weight); 
			break; 
		} 
	} 

    end = clock();
    time = (double)(end - start)/1000;

    printf("Greedy Total benefit : %.3f / CPU time : %.3fms\n", benefit, time);
    fprintf(fp, "%-5d\t\t\t\t%-5.3fms / %-20.3f", item_No, time, benefit);
}

int find_max(int a, int b){
	return (a > b)? a : b;
}

void DP(Element *ptr, int item_No, int total_weight, FILE *fp){	
	int **knapsack = (int**)malloc(sizeof(int*)*2);
	for(int i = 0; i < 2; i++)
		knapsack[i] = (int *) malloc (sizeof(int) * (total_weight + 1));
    
    int i = 0;
    int max = 0;
    time_t start, end;
    double time;

    start = clock();
 
    while (i < item_No){ 
        int j = 0;
        if(i % 2 != 0){ 
            while(++j <= total_weight){ 
                if(ptr[i].weight <= j){
                    knapsack[1][j] = find_max(ptr[i].benefit + knapsack[0][j-ptr[i].weight], knapsack[0][j]);
                }else{
                    knapsack[1][j] = knapsack[0][j]; 
                }
            }
        }else{ 
            while(++j <= total_weight){ 
                if (ptr[i].weight <= j){
                    knapsack[0][j] = find_max(ptr[i].benefit + knapsack[1][j-ptr[i].weight], knapsack[1][j]);
                }else{
                    knapsack[0][j] = knapsack[1][j];
                }
            }
        }
        i++;
    }

    end = clock();
    time = (double)(end - start)/1000;
    printf("DP Total Benefit : %d / CPU time : %.3fms\n", knapsack[1][total_weight], time);
    fprintf(fp, "%-10.3fms / %-20d", time, knapsack[1][total_weight]);

    for(i = 0; i < 2; i++)
		free(knapsack[i]);
	free(knapsack);
}

void heap_swap(Node *a, Node *b) {
    Node tmp = *a;
    *a = *b;
    *b = tmp;
}

int push(Node root, Node *heap, int total_weight) {
    if (size + 1 > total_weight) {
        return 0;
    }

    heap[size] = root;

    int current = size;
    int parent = (size - 1) / 2;

    while (current > 0 && heap[current].bound > heap[parent].bound) {
        heap_swap(&heap[current], &heap[parent]);
        current = parent;
        parent = (parent - 1) / 2;
    }

    size++;

    return 1;
}

Node pop(Node *heap) {
    if (size <= 0) {
        Node temp;
        temp.b = -1;
        return temp;
    }

    Node ret = heap[0];
    size--;

    heap[0] = heap[size];
    int current = 0;
    int leftChild = current * 2 + 1;
    int rightChild = current * 2 + 2;
    int maxNode = current;

    while (leftChild < size) {
        if (heap[maxNode].bound < heap[leftChild].bound) {
            maxNode = leftChild;
        }
        if (rightChild < size && heap[maxNode].bound < heap[rightChild].bound) {
            maxNode = rightChild;
        }

        if (maxNode == current) {
            break;
        }
        else {
            heap_swap(&heap[current], &heap[maxNode]);
            current = maxNode;
            leftChild = current * 2 + 1;
            rightChild = current * 2 + 2;
        }
    }

    return ret;
}

void BandB(Node *heap, Element *ptr, int item_No, int total_weight, FILE *fp){
    size = 0;
    int max_benefit = 0;
    int weight = 0;
    time_t start, end;
    double time;
    Node root;

    start = clock();

    root.w = 0;
    root.b = 0;
    root.index = 0;

    for(int i = 0; i < item_No; i++){
        if(weight+ptr[i].weight > total_weight){
            root.bound += (total_weight-weight)*ptr[i].density;
            break;
        }
        else {
            root.bound += ptr[i].benefit;
            weight += ptr[i].weight;
        }
    }

    push(root, heap, total_weight);

    while(1){
        Node temp = pop(heap);
        Node child[2];
        if(temp.b == -1) break;

        int index = temp.index + 1;

        if(temp.bound < max_benefit) continue;
        
        if(temp.index == item_No-1) continue;

        child[0].w = temp.w + ptr[index-1].weight;
        child[0].b = temp.b + ptr[index-1].benefit;
        child[1].w = temp.w;
        child[1].b = temp.b;

        for(int i = 0; i < 2; i++){
            child[i].index = index;
            child[i].bound = child[i].b;
            if(total_weight < child[i].w) continue;
            weight = child[i].w;
            for(int j = index; j < item_No; j++){
                if(weight + ptr[j].weight > total_weight){
                    child[i].bound += (total_weight-weight) * ptr[j].density;
                    break;
                }else{
                    child[i].bound += ptr[j].benefit;
                    weight += ptr[j].weight;
                }
            }

            if(child[i].bound < max_benefit) continue;
            if(child[i].b > max_benefit) max_benefit = child[i].b;
            push(child[i], heap, total_weight);
        }
    }

    end = clock();

    time = (double)(end - start)/1000;

    printf("BB Total Benefit : %d / CPU time : %.3fms\n", max_benefit, time);
    fprintf(fp, "%-8.3fms / %d\n", time, max_benefit);
}

int main(){
    FILE *fp; 
    int items_No = 0;
    Element *elementPtr;
    Node *heap;

    fp = fopen("output.txt", "wt");
    if(fp == NULL){
        perror("fopen 실패");
        exit(1);
    }

    fprintf(fp,"Number of\t\t\t\t\t\tProcessing time in miliseconds / Maximum benefit value\n");
    fprintf(fp,"items\t\t\t\tGreedy\t\t\t\t\t\t\tD.P.\t\t\t\t\t\t\t\tB. & B.\n");

    items_No = 10;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);


    items_No = 100;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 500;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 1000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 3000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 5000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 7000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 9000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    items_No = 10000;
    printf("Number of Items: %d\n", items_No);
    elementPtr = (Element*) malloc(sizeof(Element)*items_No);
    heap = (Node*) malloc(sizeof(Node)*items_No);

    random_number_generator(elementPtr, items_No);
    merge_sort(elementPtr, 0, items_No-1, DENSITY);
    greedy(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DESCENDING);
    DP(elementPtr, items_No, 40*items_No, fp);

    merge_sort(elementPtr, 0, items_No-1, DENSITY); 
    BandB(heap, elementPtr, items_No, 40*items_No, fp);
    printf("\n");

    free(elementPtr);
    free(heap);

    fclose(fp);
    return 0;
}