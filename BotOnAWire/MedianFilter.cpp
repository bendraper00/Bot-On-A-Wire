#include "MedianFilter.h"

void MedianFilter::push(double input){
    values[curr%N] = input;
    curr++;
}
double MedianFilter::read(){
    double sortedVals[N];
    for(int i = 0; i < N; i++){
        sortedVals[i] = values[i];
    }
    for(int i=1; i < N; i++){
        for(int j = i; j > 0; j--){
            if(sortedVals[j-1] > sortedVals[j]){
                double holder = sortedVals[j-1];
                sortedVals[j-1] = sortedVals[j];
                sortedVals[j] = holder;
            }
        }
    }
    return sortedVals[N/2];
}