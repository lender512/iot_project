#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>


#define MAX_RAW_DATA_STORAGE 64
#define RAW_SAMPLE_SIZE 8
#define MAX_SIMPLE_DATA_STORAGE (MAX_RAW_DATA_STORAGE / RAW_SAMPLE_SIZE)
#define TIME_ELAPSED_FOR_PUSH_DATA 4000
#define MAX_HEART_DATA_VALUE 300
#define MIN_HEART_DATA_VALUE 30
#define MAX_ENDO_DATA_VALUE 1023
#define MIN_ENDO_DATA_VALUE 0


#define NORMALIZE_MINMAX(x, min, max) ((x - min) / (max - min))


float raw_heart_data_storage[MAX_RAW_DATA_STORAGE];
float raw_endo_data_storage[MAX_RAW_DATA_STORAGE];
float raw_time_storage[MAX_RAW_DATA_STORAGE];
uint32_t raw_storage_size = 0;


float simple_heart_data_storage[MAX_SIMPLE_DATA_STORAGE];
float simple_endo_data_storage[MAX_SIMPLE_DATA_STORAGE];
float simple_time_storage[MAX_SIMPLE_DATA_STORAGE];
uint32_t simple_storage_size = 0;


float heart_data_average = 0;
float heart_data_standard_deviation = 0;
float endo_data_average = 0;
float endo_data_standard_deviation = 0;


void add_data(uint64_t run_id,float raw_heart_data, float raw_endo_data, float raw_time);


void push_data(uint64_t run_id);
void simplify_raw_data();
void normalize_simple_data();
void extract_features();
void send_data_to_server(uint64_t run_id);
float compute_data_average(float *data, uint32_t size);
float compute_data_standard_deviation(float average, float *data, uint32_t size);
void send_bytes_to_server(uint8_t *bytes, int size);


void add_data(uint64_t run_id, float raw_heart_data, float raw_endo_data, float raw_time){
    raw_heart_data_storage[raw_storage_size] = raw_heart_data;
    raw_endo_data_storage[raw_storage_size] = raw_endo_data;
    raw_time_storage[raw_storage_size] = raw_time;

    const float raw_time_elapsed = raw_time_storage[raw_storage_size] - raw_time_storage[0];
    raw_storage_size++;

    if(raw_time_elapsed >= TIME_ELAPSED_FOR_PUSH_DATA){
        push_data(run_id);
        raw_storage_size = 0;
        simple_storage_size = 0;
    }
}


void push_data(uint64_t run_id){
    simplify_raw_data();
    normalize_simple_data();
    extract_features();
    send_data_to_server(run_id);
}

void simplify_raw_data(){
    // Moving average with window size of RAW_SAMPLE_SIZE
    for(int i = 0; i < raw_storage_size; i++){
        float heart_data_sum = 0;
        float endo_data_sum = 0;
        float time_sum = 0;

        for(int j = 0; j < RAW_SAMPLE_SIZE; j++){
            heart_data_sum += raw_heart_data_storage[i + j];
            endo_data_sum += raw_endo_data_storage[i + j];
            time_sum += raw_time_storage[i + j];
        }

        simple_heart_data_storage[simple_storage_size] = heart_data_sum / RAW_SAMPLE_SIZE;
        simple_endo_data_storage[simple_storage_size] = endo_data_sum / RAW_SAMPLE_SIZE;
        simple_time_storage[simple_storage_size] = time_sum / RAW_SAMPLE_SIZE;

        simple_storage_size++;
    }
}

void normalize_simple_data(){
    // Normalize simple data inplace with minmax normalization
    for(int i = 0; i < simple_storage_size; i++){
        simple_heart_data_storage[i] = NORMALIZE_MINMAX(simple_heart_data_storage[i], MIN_HEART_DATA_VALUE, MAX_HEART_DATA_VALUE);
        simple_endo_data_storage[i] = NORMALIZE_MINMAX(simple_endo_data_storage[i], MIN_ENDO_DATA_VALUE, MAX_ENDO_DATA_VALUE);
    }
}


void extract_features(){
    heart_data_average = compute_data_average(simple_heart_data_storage, simple_storage_size);
    heart_data_standard_deviation = compute_data_standard_deviation(heart_data_average, simple_heart_data_storage, simple_storage_size);
    
    endo_data_average = compute_data_average(simple_endo_data_storage, simple_storage_size);
    endo_data_standard_deviation = compute_data_standard_deviation(endo_data_average, simple_endo_data_storage, simple_storage_size);
}

float compute_data_average(float *data, uint32_t size){
    float sum = 0;
    for(int i = 0; i < size; i++){
        sum += data[i];
    }
    return sum / size;
}

float compute_data_standard_deviation(float average, float *data, uint32_t size){
    float sum = 0;
    for(int i = 0; i < size; i++){
        const float difference = average - data[i];
        sum += difference * difference;
    }
    return sqrt(sum / size);
}


void send_data_to_server(uint64_t run_id){
    char buffer[512];

    memset(buffer, 0, sizeof(buffer));

    strcat(buffer, "{\"heart_avg\":");
    dtostrf(heart_data_average, 5, 3, buffer + strlen(buffer));
    strcat(buffer, ",\"heart_sd\":");
    dtostrf(heart_data_standard_deviation, 5, 3, buffer + strlen(buffer));
    strcat(buffer, ",\"endo_avg\":");
    dtostrf(endo_data_average, 5, 3, buffer + strlen(buffer));
    strcat(buffer, ",\"endo_sd\":");
    dtostrf(endo_data_standard_deviation, 5, 3, buffer + strlen(buffer));
    strcat(buffer, ",\"time\":");
    dtostrf(raw_time_storage[raw_storage_size-1], 5, 1, buffer + strlen(buffer));
    strcat(buffer, "}");

    // Print the JSON string
    Serial.println(buffer);

    send_bytes_to_server((uint8_t *) &buffer, strlen(buffer)*sizeof(char));
}

void send_bytes_to_server(uint8_t *bytes, int size){
    // Send bytes to server
    // TODO: LUIS (ARDUINO STUFF)

}

#endif
