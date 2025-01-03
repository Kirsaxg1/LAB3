#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_TOWERS_CAPACITY 10

// Structure to hold the tower and its layers
typedef struct {
    int* layers;
    int layers_count;
} Tower;


// Recursive helper function to generate all tower configurations
void generateTowers(int blocks_count, int allowed_partial_blocks_usage, int allowed_adjacent_layers_blocks_equal_count,
    Tower** towers, size_t* towers_count, size_t* towers_capacity,
    int* current_tower, int current_layer_index, int current_blocks_used, int previous_layer_blocks) {

    // Base case 1: current_blocks_used exceeds blocks_count, no valid tower can be formed
    if (current_blocks_used > blocks_count) {
        return; // stop recursion
    }

    // Base case 2: A valid tower configuration has been found
    if ((allowed_partial_blocks_usage == 0 && current_blocks_used == blocks_count) ||
        (allowed_partial_blocks_usage == 1 && current_blocks_used <= blocks_count)) {

        int num_layers = current_layer_index;
        if (num_layers > 0) {

            // Allocate memory for new tower
            Tower* new_tower = (Tower*)malloc(sizeof(Tower));
            if (new_tower == NULL) {
                // Memory allocation error
                for (size_t i = 0; i < *towers_count; i++) {
                    free(towers[i]->layers);
                    free(towers[i]);
                }
                free(towers);
                exit(4);
            }

            // Allocate memory for layers of new tower
            new_tower->layers = (int*)malloc(sizeof(int) * (num_layers + 1));
            if (new_tower->layers == NULL) {
                // Memory allocation error
                for (size_t i = 0; i < *towers_count; i++) {
                    free(towers[i]->layers);
                    free(towers[i]);
                }
                free(towers);
                free(new_tower);
                exit(4);
            }


            // Fill the layers of the new tower
            new_tower->layers[0] = num_layers;
            for (int i = 0; i < num_layers; i++) {
                new_tower->layers[i + 1] = current_tower[i];
            }
            new_tower->layers_count = num_layers;

            // Add the new tower to the results array
            towers[*towers_count] = new_tower;
            (*towers_count)++;

            // Check if towers array is full, resize if needed
            if (*towers_count == *towers_capacity) {
                *towers_capacity *= 2;
                Tower** temp_towers = (Tower**)realloc(*towers, sizeof(Tower*) * (*towers_capacity));
                if (temp_towers == NULL) {
                    // Memory allocation error
                    for (size_t i = 0; i < *towers_count; i++) {
                        free(towers[i]->layers);
                        free(towers[i]);
                    }
                    free(towers);
                    exit(4);
                }
                *towers = temp_towers;
            }
        }
        return;  // back to recursive call
    }


    // Recursive step: Try different block counts for the next layer
    for (int blocks_in_layer = 1; blocks_in_layer <= blocks_count; blocks_in_layer++) {
        // Apply constraints: blocks_in_layer >= previous layer's blocks
        if (blocks_in_layer < previous_layer_blocks) continue;

        // Apply constraints: adjacent layers cannot have equal blocks if allowed_adjacent_layers_blocks_equal_count is 0
        if (allowed_adjacent_layers_blocks_equal_count == 0 && blocks_in_layer == previous_layer_blocks && current_layer_index != 0) continue;

        // Add current blocks_in_layer to current tower config
        current_tower[current_layer_index] = blocks_in_layer;
        // Recursively try configurations with added layer
        generateTowers(blocks_count, allowed_partial_blocks_usage, allowed_adjacent_layers_blocks_equal_count,
            towers, towers_count, towers_capacity, current_tower, current_layer_index + 1, current_blocks_used + blocks_in_layer, blocks_in_layer);
    }
}


// Main function to find all tower configurations
int towers_construction(int blocks_count, int*** result_towers, size_t* result_towers_count, int allowed_partial_blocks_usage, int allowed_adjacent_layers_blocks_equal_count) {

    // Validate input parameters
    if (result_towers == NULL) return 1; // result_towers pointer is NULL
    if (result_towers_count == NULL) return 2; // result_towers_count pointer is NULL
    if (blocks_count < 0) return 3; // Negative number of blocks

    // Allocate memory for initial towers array
    Tower** towers = (Tower**)malloc(sizeof(Tower*) * INITIAL_TOWERS_CAPACITY);
    if (towers == NULL) return 4; // Memory allocation failed

    size_t towers_capacity = INITIAL_TOWERS_CAPACITY; // Initial capacity of the towers array
    *result_towers_count = 0; // Initialize number of towers found so far
    int* current_tower = (int*)malloc(sizeof(int) * blocks_count); // Allocate memory for the temporary tower config
    if (current_tower == NULL) {
        free(towers);
        return 4; // Memory allocation failed
    }

    // Start recursive tower generation
    generateTowers(blocks_count, allowed_partial_blocks_usage, allowed_adjacent_layers_blocks_equal_count,
        towers, result_towers_count, &towers_capacity, current_tower, 0, 0, 0);

    free(current_tower); // No longer need the temporary tower configuration

    // Allocate memory for the result int array
    *result_towers = (int**)malloc(sizeof(int*) * (*result_towers_count));
    if (*result_towers == NULL) {
        for (size_t i = 0; i < *result_towers_count; i++) {
            free(towers[i]->layers);
            free(towers[i]);
        }
        free(towers);
        return 4; // Memory allocation failed
    }

    // Convert the 'Tower' struct array to int array
    for (size_t i = 0; i < *result_towers_count; i++) {
        size_t layer_count = towers[i]->layers_count + 1;
        (*result_towers)[i] = (int*)malloc(sizeof(int) * layer_count);
        if ((*result_towers)[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free((*result_towers)[j]);
            }
            free(*result_towers);
            for (size_t k = 0; k < *result_towers_count; k++) {
                free(towers[k]->layers);
                free(towers[k]);
            }
            free(towers);
            return 4; // Memory allocation failed
        }
        (*result_towers)[i][0] = towers[i]->layers_count; // Save the number of layers as the first element
        for (size_t j = 0; j < towers[i]->layers_count; j++) {
            (*result_towers)[i][j + 1] = towers[i]->layers[j + 1]; // Save layer counts
        }
        free(towers[i]->layers);  // free allocated memory for each tower layer
        free(towers[i]);    // free allocated memory for each tower
    }

    free(towers); // free allocated memory for towers
    return 0;  // Function executed successfully
}


int main() {
    int blocks_count = 6;
    int** result_towers;
    size_t result_towers_count;
    int allowed_partial_blocks_usage = 0;
    int allowed_adjacent_layers_blocks_equal_count = 0;

    int errorCode = towers_construction(blocks_count, &result_towers, &result_towers_count, allowed_partial_blocks_usage, allowed_adjacent_layers_blocks_equal_count);

    if (errorCode == 0) {
        printf("Towers configurations (partial = 0, equal = 0):\n");
        for (size_t i = 0; i < result_towers_count; i++) {
            printf("Tower %zu: ", i + 1);
            for (int j = 1; j <= result_towers[i][0]; j++) {
                printf("%d ", result_towers[i][j]);
            }
            printf("\n");
            free(result_towers[i]);
        }
        free(result_towers);
    }
    else {
        fprintf(stderr, "Error: %d\n", errorCode);
    }


    blocks_count = 6;
    allowed_partial_blocks_usage = 1;
    allowed_adjacent_layers_blocks_equal_count = 1;

    errorCode = towers_construction(blocks_count, &result_towers, &result_towers_count, allowed_partial_blocks_usage, allowed_adjacent_layers_blocks_equal_count);

    if (errorCode == 0) {
        printf("Towers configurations (partial = 1, equal = 1):\n");
        for (size_t i = 0; i < result_towers_count; i++) {
            printf("Tower %zu: ", i + 1);
            for (int j = 1; j <= result_towers[i][0]; j++) {
                printf("%d ", result_towers[i][j]);
            }
            printf("\n");
            free(result_towers[i]);
        }
        free(result_towers);
    }
    else {
        fprintf(stderr, "Error: %d\n", errorCode);
    }

    return 0;
}