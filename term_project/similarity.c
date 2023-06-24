#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#define MAX_FILENAMES 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_FILENAMES];
char sub_file_list[MAX_FILENAMES][MAX_FILENAMES];

char* filenames[MAX_FILENAMES];
int num_files = 0;
int file_cnt = 0;
int main_check = 1;
int num_sub_files = 0;
int num_c_files = 1;
int dropped_try = 1;
int c_in_the_dir = 0;

// Drag and Drop
int drag_and_drop(int, char* []);
void save_filenames_to_file();
void start_button_clicked(GtkWidget*, gpointer);
void drag_data_received(GtkWidget*, GdkDragContext*, int, int, GtkSelectionData*, guint, guint, gpointer);

// Print progress bar
void *progress_bar();

// Call ls_c and Check if there is enough C files
void ls_grep_c();

// Show intro
void intro();

// Print similarity with colors
void print_similarity();

// Signal handler
void sigint_handler(int signum);
void sigquit_handler(int signum);

// main function
int main(int argc, char* argv[]) {

    // Register SIGINT, SIGQUIT signal handler
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);

    char main_dir[100];
    char sub_dir[100];

    system("clear");

    // Call the intro function
    intro();

    printf("\n======= Here is a list of files you can use =======\n");
    // Call the ls_grep_c function
    ls_grep_c();

    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".c") != NULL)
            c_in_the_dir++;
    }

    closedir(dir);

    printf("\n========= Here is a list of dropped files =========\n");
    // Call the drap_and_drop function
    drag_and_drop(argc, argv);

    FILE *dropped_file = fopen("dropped_file.txt", "r");
    if (dropped_file == NULL) {
        printf("Failed to open file: dropped_file.txt\n");
        exit(1);
    }
    for (int i = 0; i < num_files; i++) {
        fscanf(dropped_file, "%s", sub_file_list[i]);
        num_c_files++;
    }

    FILE* fp = fopen("dropped_file.txt", "r");
    if (fp == NULL) {
        printf("Failed to open file: dropped_file.txt\n");
        exit(1);
    }

    FILE* fp_result = fopen("similarity_result.txt", "w");
    if (fp_result == NULL) {
        printf("Failed to open file: similarity_result.txt\n");
        exit(1);
    }

    main_check = 1;
    for (int i = 0; i < num_files; i++) {
        // Buffer to store the file name
        char buf[100];
        fscanf(fp, "%s", buf);

        // First file is the main file
        if (main_check) {
            strcpy(main_dir, buf);
            main_check = 0;
        }
        // Other file is the sub files
        else {
            strcpy(sub_dir, buf);

            // Buffer to store the command
            char command[512];
            sprintf(command, "python3 ./similarity.py %s %s", main_dir, sub_dir);

            FILE* fp = popen(command, "r");
            if (fp == NULL) {
                printf("Error: cannot open file\n");
                return 1;
            }

            // Variable to store the similarity value
            double similarity;
            fscanf(fp, "%lf", &similarity);
            pclose(fp);

            printf("\n");

            fprintf(fp_result, "The similarity between %s and %s is %.2lf%%\n", main_dir, sub_dir, similarity);
        }
        file_cnt++;
    }
    fclose(fp);
    fclose(fp_result);

    int t = 0;
    for (t = 0; t < num_sub_files; t++) {
        if (pthread_create(&threads[t], NULL, progress_bar, (void *)sub_file_list[t+1]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", t);
            exit(1);
        }
        usleep(500000);
    }


    for (t = 0; t < num_sub_files; t++) {
        if (pthread_join(threads[t], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", t);
            exit(1);
        }
    }

    // Call the print_similarity function
    usleep(500000);
    printf("\n\n\n");
    printf("\033[1;31m"); printf("Re");
    printf("\033[1;33m"); printf("su");
    printf("\033[1;32m"); printf("lt");
    printf("\033[1;36m"); printf("i");
    printf("\033[1;34m"); printf("ng");
    printf("\033[1;35m"); printf("...");
    printf("\033[0m");

    printf("\n");
    sleep(3);
    print_similarity();

    return 0;
}

void sigint_handler(int signum) {
    // Set the text color to yellow
    printf("\033[33m");
    printf("\nCtrl-C ignored\n");
    // Reset the text color to the default
    printf("\033[0m");
}

void sigquit_handler(int signum) {
    printf("\033[33m");
    printf("\nCtrl-\\ ignored\n");
    printf("\033[0m");
}

int drag_and_drop(int argc, char* argv[]) {
    // Window widget
    GtkWidget* window;
    // Main box widget
    GtkWidget* main_box;
    // Drop label widget
    GtkWidget* drop_label;
    // Start button widget
    GtkWidget* start_button;
    // Specify the supported drag and drop target type
    GtkTargetEntry targets[] = {
        { "text/uri-list", 0, 0 }
    };

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a new top-level window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, 400, 300);
    // Connect the destroy signal to the gtk_main_quit function
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a new vertical box
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Create a new label widget
    drop_label = gtk_label_new("Drop C File here!");
    gtk_box_pack_start(GTK_BOX(main_box), drop_label, TRUE, TRUE, 0);

    // Create a new button widget
    start_button = gtk_button_new_with_label("Start Similarity Check!");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(main_box), start_button, TRUE, TRUE, 0);

    // Set drop_label as a drag destination with specified targets and actions
    gtk_drag_dest_set(drop_label, GTK_DEST_DEFAULT_ALL, targets, G_N_ELEMENTS(targets), GDK_ACTION_COPY);
    g_signal_connect(drop_label, "drag-data-received", G_CALLBACK(drag_data_received), NULL);

    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Call the save_filenames_to_file function
    save_filenames_to_file();

    for (int i = 0; i < num_files; i++) {
        g_free(filenames[i]);
    }
}

void save_filenames_to_file() {
    FILE* fp = fopen("dropped_file.txt", "w");
    if (fp == NULL) {
        printf("Failed to open file for writing.\n");
        exit(1);
    }
    for (int i = 0; i < num_files; i++) {
        fprintf(fp, "%s\n", filenames[i]);
    }
    fclose(fp);
}

void start_button_clicked(GtkWidget* widget, gpointer user_data) {
    // Quit the GTK main loop
    gtk_main_quit();
}

void drag_data_received(GtkWidget* widget, GdkDragContext* context,
    int x, int y, GtkSelectionData* data,
    guint info, guint time, gpointer user_data)
{
    char** uris = gtk_selection_data_get_uris(data);
    if (uris) {
        char** uri;
        for (uri = uris; *uri; uri++) {
            GFile* file = g_file_new_for_uri(*uri);
            char* filename = g_file_get_basename(file);
            g_object_unref(file);

            // Find the last occurrence of '.' in the filename
            char* extension = strrchr(filename, '.');
            if (extension == NULL || strcmp(extension, ".c") != 0) {
                // Set the text color to red
                printf("\033[1;31m");
                printf("Unsupported Extension: ");
                // Reset the text color to the default
                printf("\033[0m");
                printf("%s is not a C file.\n", filename);
                continue;
            }

            FILE* fp = fopen(filename, "r");
            if (fp == NULL) {
                printf("\033[1;31m");
                printf("Invalid Directory: ");
                printf("\033[0m");
                printf("%s is not in the directory.\n", filename);
                continue;
            }
            fclose(fp);

            filenames[num_files] = g_strdup(filename);

            // Print the filename as the main file & sub file
            if (main_check) {
                g_print("Main file: %s\n", filename);
                main_check = 0;
            }
            else {
                g_print("Sub file (%d): %s\n", ++num_sub_files, filename);
            }
            num_files++;
            dropped_try++;
        }
        g_strfreev(uris);
    }
    // Finish the drag operation
    gtk_drag_finish(context, TRUE, FALSE, time);
}

void *progress_bar(char *arg) {
    char *filename = (char *)arg;
    const char bar = '#';
    const char blank = ' ';
    // Length of the progress bar
    const int LEN = 50;
    const int MAX = 100;
    const int SPEED = 10;
    // Current progress count
    int count = 0;
    int i;
    // Value indicating the progress increment for each bar segment
    float tick = (float)MAX / LEN;
    int barCount;
    float percent;
    int thread_id;

    pthread_mutex_lock(&mutex);
    thread_id = file_cnt;
    file_cnt++;
    pthread_mutex_unlock(&mutex);

    while (count <= MAX) {
        int line = 16 + dropped_try + c_in_the_dir + thread_id;

        pthread_mutex_lock(&mutex);
        printf("\033[%d;0H", line);
        printf("%s ", filename);
        printf("\033[32m");
        printf("[");
        percent = (float)count / MAX * 100;
        barCount = (int)(percent / tick);
        for (i = 0; i < LEN; i++) {
            if (i < barCount)
                printf("%c", bar);
            else
                printf("%c", blank);
        }
        printf("] ");
        printf("\033[0m");
        printf("%.2f%%", percent);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);

        count++;
        usleep(SPEED * 1000);

        pthread_mutex_lock(&mutex);
        printf("\033[K");
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void ls_grep_c() {
    // Execute the 'ls_c' command and Read the output through a pipe
    FILE* pipe = popen("./ls_c", "r");
    if (pipe == NULL) {
        perror("popen");
        exit(1);
    }

    char buffer[1024];
    int c_check = 0;
    
    // Read each line from the pipe
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
        c_check++;
    }

    pclose(pipe);

    if (c_check < 2) {
        printf("There is no enough C files.\n");
        exit(1);
    }
}

// Print intro message
void intro() {
    printf("*Our Program: Similarity check\n");
    printf("*Purpose: This program checks the similarity between c codes.\n");
    printf("*Writer: mjkang, shpark, hjwi\n");
    sleep(2);

    printf("\033[1;31m     _           _ _            _ _                _               _      \n");
    printf("\033[1;31m    (_)         (_) |          (_) |              | |             | |     \n");
    printf("\033[1;33m ___ _ _ __ ___  _| | __ _ _ __ _| |_ _   _    ___| |__   ___  ___| | __  \n");
    printf("\033[1;32m/ __| | '_ ` _ \\| | |/ _` | '__| | __| | | |  / __| '_ \\ / _ \\/ __| |/ /  \n");
    printf("\033[1;36m\\__ \\ | | | | | | | | (_| | |  | | |_| |_| | | (__| | | |  __/ (__|   <   \n");
    printf("\033[1;34m|___/_|_| |_| |_|_|_|\\__,_|_|  |_|\\__|\\__, |  \\___|_| |_|\\___|\\___|_|\\_\\  \n");
    printf("\033[1;35m                                       __/ |                               \n");
    printf("\033[1;35m                                      |___/                                \n");
    printf("\033[0m");
    sleep(2);
}

// Print similarity result
void print_similarity() {
    FILE* fp_result = fopen("similarity_result.txt", "r");
    if (fp_result == NULL) {
        printf("Failed to open file: similarity_result.txt\n");
        exit(1);
    }

    printf("\033[34m");
    printf("\n\n----------------------------------");
    printf("\033[36m");
    printf("RESULT");
    printf("\033[34m");
    printf("--------------------------------\n");
    for (int i = 0; i < 2; i++) {
        printf("|");
        for (int j = 0; j < 70; j++) {
            printf(" ");
        }
        printf("|\n");
    }

    printf("\033[0m");
    for (int i = 0; i < num_sub_files; i++) {
        char text[100];
        fgets(text, sizeof(text), fp_result);

        int len = (70 - strlen(text)) / 2;
        for (int j = 0; j < 70 - len - strlen(text) - 1; j++) {
            printf(" ");
        }

        const char* keyword = "is";

        char* keyword_position = strstr(text, keyword);

        size_t before_keyword = keyword_position - text;
        printf("%.*s", (int)before_keyword, text);
        printf("%s", keyword);

        char* percentage_position = strstr(keyword_position, "%");
        char* number_start = keyword_position + strlen(keyword) + 1;
        size_t number_length = percentage_position - number_start;
        char number_string[50];
        strncpy(number_string, number_start, number_length);
        number_string[number_length] = '\0';
        double number = atof(number_string);

        const char* color_code;
        if (number > 90) {
            // Set the text color to red for high similarity
            color_code = "\033[31m";
        }
        else if (number > 70) {
            // Set the text color to magenta for medium-high similarity
            color_code = "\033[35m";
        }
        else if (number > 50) {
            // Set the text color to blue for medium similarity
            color_code = "\033[34m";
        }
        else if (number > 30) {
            // Set the text color to yellow for medium-low similarity
            color_code = "\033[33m";
        }
        else {
            // Set the text color to green for low similarity
            color_code = "\033[32m";
        }

        int offset = (int)(percentage_position - keyword_position - strlen(keyword));

        printf("%s", color_code);
        printf("%.*s", offset, keyword_position + strlen(keyword));
        printf("%%");
        printf("\033[0m\n");
    }
    printf("\033[34m");
    for (int i = 0; i < 2; i++) {
        printf("|");
        for (int j = 0; j < 70; j++) {
            printf(" ");
        }
        printf("|\n");
    }
    printf("------------------------------------------------------------------------\n");
    printf("\033[0m");
}
