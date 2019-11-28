#ifdef DEBUG
# define DEBUG_PRINT(...) fprintf( stderr, __VA_ARGS__ )
#else
# define DEBUG_PRINT(...) do{ } while (0)
#endif

/*
#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
*/

