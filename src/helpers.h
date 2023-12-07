#ifndef HEPLERS_H
#define HEPLERS_H

#define ERROR(s) fprintf(stderr, "[ERROR] in %s:%d in func %s, %s\n", __FILE__, __LINE__, __func__, s)
#define LOG(type, var)                         \
  do {                                           \
    printf("[LOG] %s:%d ", __FILE__, __LINE__); \
    printf(#var " = %" #type "\n", var);         \
  } while (0)

#define PRINT(file, s) fprintf(file, "[LOG] form %s:%d in %s says: %s\n", __FILE__, __LINE__, __func__, s)
#endif // HEPLERS_H
