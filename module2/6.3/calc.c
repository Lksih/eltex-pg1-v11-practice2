#include "calc.h"

binary_operation_t *binary_operations = NULL;
int binary_operations_count = 0;
int binary_operations_capacity = 0;

unary_operation_t *unary_operations = NULL;
int unary_operations_count = 0;
int unary_operations_capacity = 0;

loaded_lib_t *loaded_libs = NULL;
int loaded_libs_count = 0;
int loaded_libs_capacity = 0;

int add_library(void *handle, void *func, const char *name, int is_unary)
{
    if (loaded_libs_count >= loaded_libs_capacity)
    {
        int new_capacity = loaded_libs_capacity + LOADED_LIBS_CAPACITY_INCREASE_STEP;
        loaded_libs = (loaded_lib_t *)realloc(loaded_libs, new_capacity * sizeof(loaded_lib_t));
        if (!loaded_libs)
        {
            return -1;
        }
        loaded_libs_capacity = new_capacity;
    }

    if (is_unary)
    {
        if (unary_operations_count >= unary_operations_capacity)
        {
            int new_capacity = unary_operations_capacity + OPERATIONS_CAPACITY_INCREASE_STEP;
            unary_operation_t *new_array = (unary_operation_t *)realloc(unary_operations, new_capacity * sizeof(unary_operation_t));
            if (!new_array)
            {
                return -1;
            }
            unary_operations = new_array;
            unary_operations_capacity = new_capacity;
        }
    }
    else
    {
        if (binary_operations_count >= binary_operations_capacity)
        {
            int new_capacity = binary_operations_capacity + OPERATIONS_CAPACITY_INCREASE_STEP;
            binary_operation_t *new_array = (binary_operation_t *)realloc(binary_operations, new_capacity * sizeof(binary_operation_t));
            if (!new_array)
            {
                return -1;
            }
            binary_operations = new_array;
            binary_operations_capacity = new_capacity;
        }
    }

    loaded_lib_t *lib = &loaded_libs[loaded_libs_count];
    lib->handle = handle;
    lib->binary_func = is_unary ? NULL : (binary_operation_func)func;
    lib->unary_func = is_unary ? (unary_operation_func)func : NULL;
    lib->is_unary = is_unary;

    if (is_unary)
    {
        unary_operations_count++;
        unary_operations = (unary_operation_t *)realloc(unary_operations, unary_operations_count * sizeof(unary_operation_t));
        if (!unary_operations)
            return -1;

        unary_operations[unary_operations_count - 1].name = strdup(name);
        unary_operations[unary_operations_count - 1].func = lib->unary_func;
    }
    else
    {
        binary_operations_count++;
        binary_operations = (binary_operation_t *)realloc(binary_operations, binary_operations_count * sizeof(binary_operation_t));
        if (!binary_operations)
            return -1;

        binary_operations[binary_operations_count - 1].name = strdup(name);
        binary_operations[binary_operations_count - 1].func = lib->binary_func;
    }

    loaded_libs_count++;
    return 0;
}

int load_libraries(const char *lib_dir)
{
    DIR *dir = opendir(lib_dir);
    if (!dir)
    {
        perror("Не удалось открыть каталог с библиотеками");
        return -1;
    }

    struct dirent *entry;
    char path[1024];
    int loaded_count = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", lib_dir, entry->d_name);

        struct stat path_stat;
        if (stat(path, &path_stat) != 0 || !S_ISREG(path_stat.st_mode))
        {
            continue;
        }

        const char *ext = strrchr(entry->d_name, '.');
        if (!ext || strcmp(ext, ".so") != 0)
        {
            continue;
        }

        char func_name[256];
        int name_len = ext - entry->d_name - 5; // длина без "lib", ".so" и префикса типа операции
        if (name_len <= 0)
        {
            continue;
        }
        strncpy(func_name, entry->d_name + 5, name_len);
        func_name[name_len] = '\0';

        void *handle = dlopen(path, RTLD_LAZY);
        if (!handle)
        {
            fprintf(stderr, "Ошибка загрузки библиотеки %s: %s\n", path, dlerror());
            continue;
        }

        void *func = dlsym(handle, func_name);
        if (!func)
        {
            fprintf(stderr, "Не найдена функция %s в библиотеке %s: %s\n", func_name, path, dlerror());
            dlclose(handle);
            continue;
        }

        // Определяем тип функции по префиксу в имени библиотеки
        int is_unary = 0;
        if (strncmp(entry->d_name, "libu_", 5) == 0)
        {
            is_unary = 1;
        }
        else if (strncmp(entry->d_name, "libb_", 5) == 0)
        {
            is_unary = 0;
        }
        else
        {
            fprintf(stderr, "Библиотека %s не имеет префикса libu_ или libb_, пропускаем\n", entry->d_name);
            dlclose(handle);
            continue;
        }

        if (add_library(handle, func, func_name, is_unary) != 0)
        {
            dlclose(handle);
            continue;
        }

        loaded_count++;
        printf("Загружена библиотека: %s (%s)\n", entry->d_name, func_name);
    }

    closedir(dir);

    printf("Успешно загружено %d библиотек\n", loaded_count);
    return 0;
}

void unload_libraries(void)
{
    for (int i = 0; i < binary_operations_count; i++)
    {
        free(binary_operations[i].name);
    }

    free(binary_operations);
    binary_operations = NULL;
    binary_operations_count = 0;
    binary_operations_capacity = 0;

    for (int i = 0; i < unary_operations_count; i++)
    {
        free(unary_operations[i].name);
    }

    free(unary_operations);
    unary_operations = NULL;
    unary_operations_count = 0;
    unary_operations_capacity = 0;

    for (int i = 0; i < loaded_libs_count; i++)
    {
        loaded_lib_t *lib = &loaded_libs[i];
        dlclose(lib->handle);
    }

    free(loaded_libs);
    loaded_libs = NULL;
    loaded_libs_count = 0;
    loaded_libs_capacity = 0;
}