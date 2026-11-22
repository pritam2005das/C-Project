// HEADER FILES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONSTANTS
#define INT 1
#define FLOAT 2
#define DOUBLE 3
#define STRING 4
#define NAME_SIZE 100
#define VALUE_SIZE 200

// STRUCTURES
typedef struct Column {                         // Column structure
    char name[NAME_SIZE];
    int type;
    void *data;
    struct Column *next;
    struct Column *prev;
} Column;

typedef struct Table {                          // Table structure
    int table_id;
    char name[NAME_SIZE];
    Column *columns;
    int column_count;
    int row_count;
    int next_id;
    struct Table *next;
    struct Table *prev;
} Table;

// GLOBAL VARIABLES
Table *tables = NULL;
int next_table_id = 1;
char database_name[NAME_SIZE];

// INPUT FUNCTIONS

void clear_input() {                             // Clear input buffer
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void read_string(char message[], char value[]) { // Read string input
    printf("%s", message);
    if (fgets(value, VALUE_SIZE, stdin) != NULL) {
        if (strchr(value, '\n') != NULL) value[strcspn(value, "\n")] = '\0';
        else clear_input();
    }
}

int read_integer(char message[]) {               // Read integer input
    int value = 0;
    printf("%s", message);
    scanf("%d", &value);
    clear_input();
    return value;
}

float read_float(char message[]) {               // Read float input
    float value = 0;
    printf("%s", message);
    scanf("%f", &value);
    clear_input();
    return value;
}

double read_double(char message[]) {             // Read double input
    double value = 0;
    printf("%s", message);
    scanf("%lf", &value);
    clear_input();
    return value;
}

// DATA TYPE FUNCTIONS

void show_data_types() {                         // Display available data types
    printf("\n1. INT");
    printf("\n2. FLOAT");
    printf("\n3. DOUBLE");
    printf("\n4. STRING\n");
}

int get_data_size(int type) {                    // Get size of data type
    if (type == INT) return sizeof(int);
    if (type == FLOAT) return sizeof(float);
    if (type == DOUBLE) return sizeof(double);
    if (type == STRING) return sizeof(char *);
    return 0;
}

char *get_type_name(int type) {                  // Get data type name
    if (type == INT) return "INT";
    if (type == FLOAT) return "FLOAT";
    if (type == DOUBLE) return "DOUBLE";
    if (type == STRING) return "STRING";
    return "UNKNOWN";
}

// COLUMN FUNCTIONS

Column *create_column(char name[], int type, int rows) { // Create a column
    int i, j;
    Column *column = malloc(sizeof(Column));
    if (column == NULL) return NULL;
    strcpy(column->name, name);
    column->type = type;
    column->data = NULL;
    column->next = column;
    column->prev = column;
    if (rows > 0) {
        column->data = calloc(rows, get_data_size(type));
        if (column->data == NULL) {
            free(column);
            return NULL;
        }
        if (type == STRING) {
            char **values = column->data;
            for (i = 0; i < rows; i++) {
                values[i] = calloc(VALUE_SIZE, sizeof(char));
                if (values[i] == NULL) {
                    for (j = 0; j < i; j++) free(values[j]);
                    free(column->data);
                    free(column);
                    return NULL;
                }
            }
        }
    }
    return column;
}

void insert_column(Table *table, Column *column) { // Insert column
    if (table->columns == NULL) table->columns = column;
    else {
        Column *last = table->columns->prev;
        last->next = column;
        column->prev = last;
        column->next = table->columns;
        table->columns->prev = column;
    }
    table->column_count++;
}

Column *find_column(Table *table, char name[]) {  // Find column by name
    Column *current;
    if (table->columns == NULL) return NULL;
    current = table->columns;
    do {
        if (strcmp(current->name, name) == 0) return current;
        current = current->next;
    } while (current != table->columns);
    return NULL;
}

void free_column(Column *column, int rows) {     // Free column memory
    int i;
    if (column->type == STRING) {
        char **values = column->data;
        for (i = 0; i < rows; i++) free(values[i]);
    }
    free(column->data);
    free(column);
}

// TABLE FUNCTIONS

Table *create_new_table(char name[]) {           // Create a new table
    Table *table = malloc(sizeof(Table));
    Column *id_column;
    if (table == NULL) return NULL;
    table->table_id = next_table_id++;
    strcpy(table->name, name);
    table->columns = NULL;
    table->column_count = 0;
    table->row_count = 0;
    table->next_id = 1;
    table->next = table;
    table->prev = table;
    id_column = create_column("id", INT, 0);
    if (id_column == NULL) {
        free(table);
        return NULL;
    }
    insert_column(table, id_column);
    return table;
}

void insert_table(Table *table) {                // Insert table
    Table *last;
    if (tables == NULL) tables = table;
    else {
        last = tables->prev;
        last->next = table;
        table->prev = last;
        table->next = tables;
        tables->prev = table;
    }
}

Table *find_table(char name[]) {                 // Find table by name
    Table *current;
    if (tables == NULL) return NULL;
    current = tables;
    do {
        if (strcmp(current->name, name) == 0) return current;
        current = current->next;
    } while (current != tables);
    return NULL;
}

void free_table(Table *table) {                  // Free table memory
    Column *current, *next;
    if (table->columns != NULL) {
        current = table->columns;
        do {
            next = current->next;
            free_column(current, table->row_count);
            current = next;
        } while (current != table->columns);
    }
    free(table);
}

// FILE HANDLING

void save_database() {                           // Save database to file
    FILE *file;
    Table *table;
    Column *column;
    int table_count = 0, i, j;
    unsigned int length;
    char *value;

    file = fopen(database_name, "wb");
    if (file == NULL) {
        printf("\nUnable to save database.\n");
        return;
    }
    if (tables != NULL) {
        table = tables;
        do {
            table_count++;
            table = table->next;
        } while (table != tables);
    }
    fwrite("CDB1", 1, 4, file);
    fwrite(&next_table_id, sizeof(int), 1, file);
    fwrite(&table_count, sizeof(int), 1, file);
    if (tables != NULL) {
        table = tables;
        do {
            fwrite(&table->table_id, sizeof(int), 1, file);
            fwrite(table->name, sizeof(char), NAME_SIZE, file);
            fwrite(&table->column_count, sizeof(int), 1, file);
            fwrite(&table->row_count, sizeof(int), 1, file);
            fwrite(&table->next_id, sizeof(int), 1, file);
            column = table->columns;
            for (i = 0; i < table->column_count; i++) {
                fwrite(column->name, sizeof(char), NAME_SIZE, file);
                fwrite(&column->type, sizeof(int), 1, file);
                for (j = 0; j < table->row_count; j++) {
                    if (column->type == INT) fwrite(&((int *)column->data)[j], sizeof(int), 1, file);
                    else if (column->type == FLOAT) fwrite(&((float *)column->data)[j], sizeof(float), 1, file);
                    else if (column->type == DOUBLE) fwrite(&((double *)column->data)[j], sizeof(double), 1, file);
                    else if (column->type == STRING) {
                        value = ((char **)column->data)[j];
                        length = value == NULL ? 0 : strlen(value);
                        fwrite(&length, sizeof(unsigned int), 1, file);
                        if (length > 0) fwrite(value, sizeof(char), length, file);
                    }
                }
                column = column->next;
            }
            table = table->next;
        } while (table != tables);
    }
    fclose(file);
}

void load_database() {                           // Load database from file
    FILE *file;
    char header[5] = {0};
    char table_name[NAME_SIZE];
    char column_name[NAME_SIZE];
    int table_count, table_id, column_count, row_count, next_id;
    int i, j, k, type, valid;
    unsigned int length;
    Table *table;
    Column *column;
    char **values;

    file = fopen(database_name, "rb");
    if (file == NULL) return;
    if (fread(header, 1, 4, file) != 4 || strcmp(header, "CDB1") != 0) {
        fclose(file);
        printf("\nInvalid database file.\n");
        return;
    }
    if (fread(&next_table_id, sizeof(int), 1, file) != 1 ||
        fread(&table_count, sizeof(int), 1, file) != 1 ||
        table_count < 0 || table_count > 10000) {
        fclose(file);
        printf("\nInvalid database file.\n");
        return;
    }
    for (i = 0; i < table_count; i++) {
        if (fread(&table_id, sizeof(int), 1, file) != 1 ||
            fread(table_name, sizeof(char), NAME_SIZE, file) != NAME_SIZE ||
            fread(&column_count, sizeof(int), 1, file) != 1 ||
            fread(&row_count, sizeof(int), 1, file) != 1 ||
            fread(&next_id, sizeof(int), 1, file) != 1) break;
        if (column_count < 1 || column_count > 1000 || row_count < 0 || row_count > 100000) break;
        table_name[NAME_SIZE - 1] = '\0';
        table = malloc(sizeof(Table));
        if (table == NULL) break;
        table->table_id = table_id;
        strcpy(table->name, table_name);
        table->columns = NULL;
        table->column_count = 0;
        table->row_count = row_count;
        table->next_id = next_id;
        table->next = table;
        table->prev = table;
        valid = 1;
        for (j = 0; j < column_count && valid; j++) {
            if (fread(column_name, sizeof(char), NAME_SIZE, file) != NAME_SIZE ||
                fread(&type, sizeof(int), 1, file) != 1 ||
                type < INT || type > STRING) {
                valid = 0;
                break;
            }
            column_name[NAME_SIZE - 1] = '\0';
            column = create_column(column_name, type, row_count);
            if (column == NULL) {
                valid = 0;
                break;
            }
            for (k = 0; k < row_count && valid; k++) {
                if (type == INT && fread(&((int *)column->data)[k], sizeof(int), 1, file) != 1) valid = 0;
                else if (type == FLOAT && fread(&((float *)column->data)[k], sizeof(float), 1, file) != 1) valid = 0;
                else if (type == DOUBLE && fread(&((double *)column->data)[k], sizeof(double), 1, file) != 1) valid = 0;
                else if (type == STRING) {
                    values = column->data;
                    if (fread(&length, sizeof(unsigned int), 1, file) != 1 || length >= VALUE_SIZE) valid = 0;
                    else if (length > 0 && fread(values[k], sizeof(char), length, file) != length) valid = 0;
                    else values[k][length] = '\0';
                }
            }
            if (valid) insert_column(table, column);
            else free_column(column, row_count);
        }
        if (valid && table->column_count == column_count) insert_table(table);
        else free_table(table);
    }
    fclose(file);
}

// DISPLAY FUNCTIONS

void show_tables() {                             // Display all tables
    Table *current;
    if (tables == NULL) {
        printf("\nNo tables available.\n");
        return;
    }
    printf("\n%-10s %-25s %-15s %-15s\n", "TABLE ID", "TABLE NAME", "COLUMNS", "RECORDS");
    printf("---------------------------------------------------------------------\n");
    current = tables;
    do {
        printf("%-10d %-25s %-15d %-15d\n", current->table_id, current->name, current->column_count, current->row_count);
        current = current->next;
    } while (current != tables);
}

void show_table_details(Table *table) {          // Display table details
    int i;
    Column *column;
    printf("\nTABLE DETAILS");
    printf("\nTable ID       : %d", table->table_id);
    printf("\nTable Name     : %s", table->name);
    printf("\nColumn Count   : %d", table->column_count);
    printf("\nRecord Count   : %d", table->row_count);
    printf("\nPrimary Key    : id (INT)\n");
    printf("\n%-20s %-15s\n", "COLUMN NAME", "DATA TYPE");
    printf("-----------------------------------\n");
    column = table->columns;
    for (i = 0; i < table->column_count; i++) {
        printf("%-20s %-15s\n", column->name, get_type_name(column->type));
        column = column->next;
    }
}

void display_value(Column *column, int index) {  // Display a column value
    if (column->type == INT) printf("%-20d", ((int *)column->data)[index]);
    else if (column->type == FLOAT) printf("%-20.2f", ((float *)column->data)[index]);
    else if (column->type == DOUBLE) printf("%-20.2lf", ((double *)column->data)[index]);
    else if (column->type == STRING) printf("%-20s", ((char **)column->data)[index]);
}

void show_table(Table *table) {                  // Display table records
    int i, j;
    Column *column;
    printf("\nTABLE: %s\n", table->name);
    column = table->columns;
    for (i = 0; i < table->column_count; i++) {
        printf("%-20s", column->name);
        column = column->next;
    }
    printf("\n");
    for (i = 0; i < table->column_count; i++) printf("--------------------");
    printf("\n");
    for (i = 0; i < table->row_count; i++) {
        column = table->columns;
        for (j = 0; j < table->column_count; j++) {
            display_value(column, i);
            column = column->next;
        }
        printf("\n");
    }
    printf("\nTotal Records: %d\n", table->row_count);
}

// TABLE MANAGEMENT

void create_table() {                            // Create a new table
    char name[VALUE_SIZE];
    Table *table;
    read_string("\nEnter table name: ", name);
    if (strlen(name) == 0 || strlen(name) >= NAME_SIZE) {
        printf("\nInvalid table name.\n");
        return;
    }
    if (find_table(name) != NULL) {
        printf("\nTable already exists.\n");
        return;
    }
    table = create_new_table(name);
    if (table == NULL) {
        printf("\nUnable to create table.\n");
        return;
    }
    insert_table(table);
    save_database();
    printf("\nTable created successfully. Table ID: %d\n", table->table_id);
}

Table *select_table() {                          // Select table by name
    char name[VALUE_SIZE];
    Table *table;
    read_string("\nEnter table name: ", name);
    table = find_table(name);
    if (table == NULL) printf("\nTable not found.\n");
    return table;
}

void delete_table() {                            // Delete a table
    Table *table;
    char confirmation[VALUE_SIZE];
    table = select_table();
    if (table == NULL) return;
    read_string("Are you sure? (yes/no): ", confirmation);
    if (strcmp(confirmation, "yes") != 0) {
        printf("\nDeletion cancelled.\n");
        return;
    }
    if (table->next == table) tables = NULL;
    else {
        table->prev->next = table->next;
        table->next->prev = table->prev;
        if (tables == table) tables = table->next;
    }
    free_table(table);
    save_database();
    printf("\nTable deleted successfully.\n");
}

// COLUMN MANAGEMENT

void add_column(Table *table) {                  // Add a new column
    char name[VALUE_SIZE];
    int type;
    Column *column;
    read_string("\nEnter column name: ", name);
    if (strlen(name) == 0 || strlen(name) >= NAME_SIZE ||
        strcmp(name, "id") == 0 || find_column(table, name) != NULL) {
        printf("\nInvalid or duplicate column name.\n");
        return;
    }
    show_data_types();
    type = read_integer("Select data type: ");
    if (type < INT || type > STRING) {
        printf("\nInvalid data type.\n");
        return;
    }
    column = create_column(name, type, table->row_count);
    if (column == NULL) {
        printf("\nUnable to create column.\n");
        return;
    }
    insert_column(table, column);
    save_database();
    printf("\nColumn added successfully.\n");
}

void delete_column(Table *table) {               // Delete a column
    char name[VALUE_SIZE];
    Column *column;
    read_string("\nEnter column name: ", name);
    if (strcmp(name, "id") == 0) {
        printf("\nPrimary key column cannot be deleted.\n");
        return;
    }
    column = find_column(table, name);
    if (column == NULL) {
        printf("\nColumn not found.\n");
        return;
    }
    column->prev->next = column->next;
    column->next->prev = column->prev;
    free_column(column, table->row_count);
    table->column_count--;
    save_database();
    printf("\nColumn deleted successfully.\n");
}

// RECORD MANAGEMENT

void add_record(Table *table) {                  // Add a new record
    int count, index, i, success;
    Column *column;
    void *new_data;
    char value[VALUE_SIZE];

    count = table->column_count;
    index = table->row_count;
    success = 1;
    column = table->columns;

    for (i = 0; i < count; i++) {
        new_data = realloc(column->data, (index + 1) * get_data_size(column->type));
        if (new_data == NULL) {
            success = 0;
            break;
        }
        column->data = new_data;
        if (column->type == STRING) ((char **)column->data)[index] = NULL;
        column = column->next;
    }

    if (!success) {
        printf("\nUnable to allocate memory for record.\n");
        return;
    }

    column = table->columns;
    for (i = 0; i < count; i++) {
        if (strcmp(column->name, "id") == 0) ((int *)column->data)[index] = table->next_id;
        else {
            printf("\nColumn: %s (%s)\n", column->name, get_type_name(column->type));
            if (column->type == INT) ((int *)column->data)[index] = read_integer("Enter value: ");
            else if (column->type == FLOAT) ((float *)column->data)[index] = read_float("Enter value: ");
            else if (column->type == DOUBLE) ((double *)column->data)[index] = read_double("Enter value: ");
            else if (column->type == STRING) {
                read_string("Enter value: ", value);
                ((char **)column->data)[index] = malloc(strlen(value) + 1);
                if (((char **)column->data)[index] == NULL) success = 0;
                else strcpy(((char **)column->data)[index], value);
            }
        }
        column = column->next;
        if (!success) break;
    }

    if (!success) {
        column = table->columns;
        for (i = 0; i < count; i++) {
            if (column->type == STRING) {
                free(((char **)column->data)[index]);
                ((char **)column->data)[index] = NULL;
            }
            column = column->next;
        }
        printf("\nUnable to add record.\n");
        return;
    }

    table->row_count++;
    table->next_id++;
    save_database();
    printf("\nRecord added successfully. ID: %d\n", table->next_id - 1);
}

int find_record_index(Table *table, int id) {    // Find record index by ID
    int i;
    int *ids = table->columns->data;
    for (i = 0; i < table->row_count; i++) {
        if (ids[i] == id) return i;
    }
    return -1;
}

void display_record(Table *table, int index) {   // Display a single record
    int i;
    Column *column = table->columns;
    printf("\n");
    for (i = 0; i < table->column_count; i++) {
        printf("%s: ", column->name);
        display_value(column, index);
        printf("\n");
        column = column->next;
    }
}

void find_record(Table *table) {                 // Find record by ID
    int id, index;
    id = read_integer("\nEnter record ID: ");
    index = find_record_index(table, id);
    if (index == -1) printf("\nRecord not found.\n");
    else display_record(table, index);
}

void write_value(Column *column, int index, char value[]) { // Update a value
    int new_int;
    float new_float;
    double new_double;
    char *new_value;

    if (column->type == INT) {
        if (sscanf(value, "%d", &new_int) == 1) ((int *)column->data)[index] = new_int;
        else printf("\nInvalid integer. Value unchanged.\n");
    }
    else if (column->type == FLOAT) {
        if (sscanf(value, "%f", &new_float) == 1) ((float *)column->data)[index] = new_float;
        else printf("\nInvalid float. Value unchanged.\n");
    }
    else if (column->type == DOUBLE) {
        if (sscanf(value, "%lf", &new_double) == 1) ((double *)column->data)[index] = new_double;
        else printf("\nInvalid double. Value unchanged.\n");
    }
    else if (column->type == STRING) {
        new_value = malloc(strlen(value) + 1);
        if (new_value != NULL) {
            strcpy(new_value, value);
            free(((char **)column->data)[index]);
            ((char **)column->data)[index] = new_value;
        }
    }
}

void edit_record(Table *table) {                 // Edit record by ID
    int id, index, i;
    char value[VALUE_SIZE];
    Column *column;

    id = read_integer("\nEnter record ID: ");
    index = find_record_index(table, id);
    if (index == -1) {
        printf("\nRecord not found.\n");
        return;
    }
    column = table->columns;
    for (i = 0; i < table->column_count; i++) {
        if (strcmp(column->name, "id") != 0) {
            printf("\nColumn: %s (%s)\n", column->name, get_type_name(column->type));
            read_string("Enter new value (Leave blank to keep unchanged): ", value);
            if (strcmp(value, "") != 0) write_value(column, index, value);
        }
        column = column->next;
    }
    save_database();
    printf("\nRecord updated successfully.\n");
}

void delete_record(Table *table) {               // Delete record by ID
    int id, index, i, j, size;
    Column *column;
    char *values;
    void *new_data;

    id = read_integer("\nEnter record ID: ");
    index = find_record_index(table, id);
    if (index == -1) {
        printf("\nRecord not found.\n");
        return;
    }
    column = table->columns;
    for (i = 0; i < table->column_count; i++) {
        if (column->type == STRING) {
            char **strings = column->data;
            free(strings[index]);
            for (j = index; j < table->row_count - 1; j++) strings[j] = strings[j + 1];
        }
        else {
            size = get_data_size(column->type);
            values = column->data;
            for (j = index; j < table->row_count - 1; j++) {
                memcpy(values + j * size, values + (j + 1) * size, size);
            }
        }
        column = column->next;
    }
    table->row_count--;
    column = table->columns;
    for (i = 0; i < table->column_count; i++) {
        if (table->row_count == 0) {
            free(column->data);
            column->data = NULL;
        }
        else {
            new_data = realloc(column->data, table->row_count * get_data_size(column->type));
            if (new_data != NULL) column->data = new_data;
        }
        column = column->next;
    }
    save_database();
    printf("\nRecord deleted successfully.\n");
}

// FILTER FUNCTIONS

int matches_filter(Column *column, int index, char filter[]) { // Check filter condition
    int int_value;
    float float_value;
    double double_value;

    if (column->type == INT) {
        if (sscanf(filter, "%d", &int_value) != 1) return 0;
        return ((int *)column->data)[index] == int_value;
    }
    if (column->type == FLOAT) {
        if (sscanf(filter, "%f", &float_value) != 1) return 0;
        return ((float *)column->data)[index] == float_value;
    }
    if (column->type == DOUBLE) {
        if (sscanf(filter, "%lf", &double_value) != 1) return 0;
        return ((double *)column->data)[index] == double_value;
    }
    if (column->type == STRING) return strcmp(((char **)column->data)[index], filter) == 0;
    return 0;
}

void filter_records(Table *table) {              // Find records using multiple filters
    int i, j, found, match;
    char (*filters)[VALUE_SIZE];
    Column *column;

    filters = malloc(table->column_count * VALUE_SIZE);
    if (filters == NULL) return;
    column = table->columns;
    printf("\nLeave blank for columns you do not want to filter.\n");
    for (i = 0; i < table->column_count; i++) {
        printf("\nColumn: %s (%s)\n", column->name, get_type_name(column->type));
        read_string("Enter filter value: ", filters[i]);
        column = column->next;
    }
    found = 0;
    for (i = 0; i < table->row_count; i++) {
        match = 1;
        column = table->columns;
        for (j = 0; j < table->column_count; j++) {
            if (strcmp(filters[j], "") != 0 && !matches_filter(column, i, filters[j])) {
                match = 0;
                break;
            }
            column = column->next;
        }
        if (match) {
            display_record(table, i);
            found++;
        }
    }
    if (found == 0) printf("\nNo matching records found.\n");
    else printf("\nMatching Records: %d\n", found);
    free(filters);
}

// EDIT TABLE MENU

void edit_table_menu(Table *table) {             // Manage table operations
    int choice;
    do {
        printf("\nEDIT TABLE: %s\n", table->name);
        printf("\nMenu:-\n", database_name);
        printf("--------", database_name);
        printf("\n1. Add Column");
        printf("\n2. Delete Column");
        printf("\n3. Add Record");
        printf("\n4. Find Record");
        printf("\n5. Edit Record");
        printf("\n6. Delete Record");
        printf("\n7. Find Records Using Filters");
        printf("\n8. Exit");
        choice = read_integer("\n\nEnter choice: ");
        if (choice == 1) add_column(table);
        else if (choice == 2) delete_column(table);
        else if (choice == 3) add_record(table);
        else if (choice == 4) find_record(table);
        else if (choice == 5) edit_record(table);
        else if (choice == 6) delete_record(table);
        else if (choice == 7) filter_records(table);
        else if (choice != 8) printf("\nInvalid choice.\n");
    } while (choice != 8);
}

// DATABASE FUNCTIONS

void database_details() {                        // Display database details
    int count = 0;
    Table *current;
    if (tables != NULL) {
        current = tables;
        do {
            count++;
            current = current->next;
        } while (current != tables);
    }
    printf("\nDATABASE DETAILS");
    printf("\nDatabase Name : %s", database_name);
    printf("\nTable Count   : %d\n", count);
}

void free_database() {                           // Free complete database
    Table *current, *next;
    if (tables == NULL) return;
    current = tables->next;
    while (current != tables) {
        next = current->next;
        free_table(current);
        current = next;
    }
    free_table(tables);
    tables = NULL;
}

// MAIN FUNCTION

int main(int argc, char *argv[]) {               // Main program
    int choice;
    Table *table;

    if (argc > 1) {
        strncpy(database_name, argv[1], NAME_SIZE - 1);
        database_name[NAME_SIZE - 1] = '\0';
    }
    else strcpy(database_name, "database.db");

    load_database();
    printf("\nWelcome to %s\n", database_name);
    do {
    	printf("\nMenu:-\n", database_name);
    	printf("---------", database_name);
        printf("\n1. Create Table");
        printf("\n2. Show Tables");
        printf("\n3. Show Table");
        printf("\n4. Show Table Details");
        printf("\n5. Edit Table");
        printf("\n6. Delete Table");
        printf("\n7. Database Details");
        printf("\n8. Exit");
        choice = read_integer("\n\nEnter choice: ");
        if (choice == 1) create_table();
        else if (choice == 2) show_tables();
        else if (choice == 3) {
            table = select_table();
            if (table != NULL) show_table(table);
        }
        else if (choice == 4) {
            table = select_table();
            if (table != NULL) show_table_details(table);
        }
        else if (choice == 5) {
            table = select_table();
            if (table != NULL) edit_table_menu(table);
        }
        else if (choice == 6) delete_table();
        else if (choice == 7) database_details();
        else if (choice == 8) printf("\nExiting database...\n");
        else printf("\nInvalid choice.\n");
    } while (choice != 8);

    save_database();
    free_database();
    return 0;
}