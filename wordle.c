// Wordle Helper
// Author: Rohith Kotia Palakirti

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <csse2310a1.h>

#define MAX_SIZE 10
#define MAX_WORD_SIZE 50
#define MIN_LEN 4
#define MAX_LEN 9
#define DEFAULT_LEN 5

/*
* ./wordle-helper [-alpha|-best] [-len word-length ] [-with letters ] \
* [-without letters ] [pattern ]
*/

/*
* Struct Definition
*/
typedef struct {
    int len;
    int lenFlag, withFlag, withoutFlag, alphaFlag, bestFlag, patternFlag,
            foundMatch;
    char withString[MAX_SIZE], withoutString[MAX_SIZE],
            patternString[MAX_SIZE];
} WordleArguments;

/*
* Function Prototypes
*/
void print_std_err(int value);
int is_valid_argument(char arg);
int check_arg(char *arg);
void convert_word_to_uppercase(char dictionaryString[], int *foundSpecial);
WordleArguments detect_len_arg(int argc, char *arg, int flag);
WordleArguments detect_with_arg(int argc, char *arg, int withFlag);
WordleArguments detect_without_arg(int argc, char *arg, int flag);
WordleArguments detect_alpha_arg(int argc, char *arg, int alphaFlag,
        int bestFlag);
WordleArguments detect_best_arg(int argc, char *arg, int bestFlag,
        int alphaFlag);
WordleArguments detect_pattern_arg(int argc, char *arg, int patternFlag,
        int len);
FILE *open_dictionary();
void compare_with_dictionary(FILE *dictionary, int len, char withString[],
        char withoutString[], char patternString[],
        int withFlag, int withoutFlag, int patternFlag,
        int alphaFlag, int bestFlag);
WordleArguments parse_commandline_args(int argc, char *argv[]);
void filter_words_matching_pattern(char patternString[], 
        char dictionaryString[], int *matchesPattern);
void filter_words_matching_with(char withString[], char dictionaryString[],
        int *matchesWith);
void filter_words_matching_without(char withoutString[], 
        char dictionaryString[], int *matchesWithout);
void set_default_len(int *lenFlag, int *len);
void sort_words_alphabetically(char **words, int count);
void sort_words_by_best_match(char **words, int count);
void print_non_duplicate_words(char **words, int count);
void free_memory(char **words, int *count);

/* int main(int argc, char *argv[])
* -----------------------------------------------
* Initiates and runs the program
*
* argc: count of number of commandline arguments
* argv: the array of commandline arguments stored as strings
*
* Returns: exit code of the program
* Errors: programs exits if the input is invalid
*/
int main(int argc, char *argv[])
{
    char withString[MAX_SIZE], withoutString[MAX_SIZE],
            patternString[MAX_SIZE];
    int patternFlag = 0;
    // Parsing command line arguments
    WordleArguments p = parse_commandline_args(argc, argv);
    int withFlag = p.withFlag;
    int withoutFlag = p.withoutFlag;
    int alphaFlag = p.alphaFlag;
    int bestFlag = p.bestFlag;
    int len = p.len;
    strcpy(withString, p.withString);
    strcpy(withoutString, p.withoutString);
    // Parsing pattern
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && (argv[i - 1][0] != '-'
                || (strcmp(argv[i - 1], "-alpha") == 0)
                || (strcmp(argv[i - 1], "-best") == 0))) {
            WordleArguments patternStruct =
                    detect_pattern_arg(argc, argv[i], patternFlag, len);
            patternFlag = patternStruct.patternFlag;
            strcpy(patternString, patternStruct.patternString);
        }
    }
    // Opening dictionary file
    FILE *dictionary;
    dictionary = open_dictionary();
    // Searching for pattern in dictionary
    compare_with_dictionary(dictionary, len, withString, withoutString,
            patternString, withFlag, withoutFlag, patternFlag,
            alphaFlag, bestFlag);
    fclose(dictionary);
    return 0;
}

/* WordleArguments parse_commandline_args(int argc, char *argv[])
* -----------------------------------------------
* Parses the commandline arguments that have been passed, sets flags and stores
* parsed values
*
* argc: count of number of commandline arguments
* argv: the array of commandline arguments stored as strings

* Returns: A struct with all of the parsed values and flags
* Errors: exits with code 4, if the same argument has been passed more than 
* once.
*/
WordleArguments parse_commandline_args(int argc, char *argv[])
{   
    WordleArguments args;
    args.lenFlag = args.withFlag = args.withoutFlag = args.alphaFlag = 
            args.bestFlag = args.patternFlag = args.foundMatch = 0;
    int lenFlag, withFlag, withoutFlag, alphaFlag, bestFlag, patternFlag, 
            foundMatch; 
    lenFlag = withFlag = withoutFlag = alphaFlag = bestFlag = patternFlag 
            = foundMatch = 0;
    strcpy(args.withString, "");
    strcpy(args.withoutString, "");
    for (int i = 1; i < argc; i++) {
        if (is_valid_argument(argv[i][0])) {
            if (check_arg(argv[i])) {
                print_std_err(1);
            }
            if (strcmp(argv[i], "-len") == 0) { // Detecting -len arg
                WordleArguments lenStruct =
                        detect_len_arg(argc, argv[i + 1], lenFlag);
                args.len = lenStruct.len;
                lenFlag = args.lenFlag = lenStruct.lenFlag;
            }
            if (strcmp(argv[i], "-with") == 0) { // Detecting -with arg
                WordleArguments withStruct =
                        detect_with_arg(argc, argv[i + 1], args.withFlag);
                withFlag = args.withFlag = withStruct.withFlag;
                strcpy(args.withString, withStruct.withString);
            }
            if (strcmp(argv[i], "-without") == 0) { // Detecting -without arg
                WordleArguments withoutStruct =
                        detect_without_arg(argc, argv[i + 1], 
                        args.withoutFlag);
                withoutFlag = args.withoutFlag = withoutStruct.withoutFlag;
                strcpy(args.withoutString, withoutStruct.withoutString);
            }
            if (strcmp(argv[i], "-alpha") == 0) { // Detecting -alpha arg
                WordleArguments alphaStruct =
                        detect_alpha_arg(argc, argv[i], args.alphaFlag,
                        args.bestFlag);
                alphaFlag = args.alphaFlag = alphaStruct.alphaFlag;
            }
            if (strcmp(argv[i], "-best") == 0) { // Detecting -best arg
                WordleArguments bestStruct =
                        detect_best_arg(argc, argv[i], args.bestFlag,
                        args.alphaFlag);
                bestFlag = args.bestFlag = bestStruct.bestFlag;
            }
        }
    }
    set_default_len(&args.lenFlag, &args.len);
    return args;
}

/* void compare_with_dictionary(FILE *dictionary, int len,
        char withString[], char withoutString[],
        char patternString[], int withFlag,
        int withoutFlag, int patternFlag, int alphaFlag,
        int bestFlag)
* -----------------------------------------------
* Compares the parsed pattern, with and without characters against the words
* from the dictionary. If a match is found, the word is printed to stdout.
*
* dictionary: dictionary file that is being read
* len: length argument inputted by user
* withString, withoutString, patternString: the parsed argument values
* withFlag, withoutFlag patternFlag, alphaFlag, bestFlag: flags that have been
*                                                         set
*
* Returns: -
* Errors: exits with code 4, if no match has been found in the dictionary
*/
void compare_with_dictionary(FILE *dictionary, int len,
        char withString[], char withoutString[],
        char patternString[], int withFlag,
        int withoutFlag, int patternFlag, int alphaFlag,
        int bestFlag)
{
    int foundMatch = 0;
    int count = 0;
    int arraySize = 1;
    char **words = calloc(1, sizeof(char *));
    while (!feof(dictionary)) {
        int foundSpecial = 0;
        int matchesPattern = 1;
        int matchesWith = 1;
        int matchesWithout = 0;
        char dictionaryString[MAX_WORD_SIZE];
        fscanf(dictionary, "%s", dictionaryString);
        convert_word_to_uppercase(dictionaryString, &foundSpecial);
        // Check word length and check if word contains only letters
        if (strlen(dictionaryString) == len) {
            if (foundSpecial == 0) {
                // If pattern argument is given, check for the pattern & filter
                if (patternFlag) {
                    filter_words_matching_pattern(patternString, 
                            dictionaryString, &matchesPattern);
                }
                if (matchesPattern) {
                    // Filter words by including words that contain letters
                    // specified in the -with argument
                    if (withFlag) {
                        filter_words_matching_with(withString,
                                dictionaryString, &matchesWith);
                    }
                    if (matchesWith) {
                        // Filter out words that contain letters specified
                        // in the -without argument
                        if (withoutFlag) {
                            filter_words_matching_without(withoutString,
                                    dictionaryString, &matchesWithout);
                        }
                        if (!matchesWithout) {
                            foundMatch = 1;
                            if (count == arraySize - 1) {
                                arraySize = arraySize * 2;
                                words = (char **) realloc(words, 
                                        sizeof(*words) * arraySize);
                            }
                            words[count] = strdup(dictionaryString);
                            // Reference: use of strdup/array size doubling
                            // was inspired from
                            // https://stackoverflow.com/questions/72062726/
                            // how-to-properly-reallocate-an-array-of-strings
                            count++;
                        }
                    }
                }
            }
        }
    }
    if (foundMatch == 0) {
        free_memory(words, &count);
        fclose(dictionary);
        exit(4);
    } else if (alphaFlag) {
        sort_words_alphabetically(words, count);
        print_non_duplicate_words(words, count);
    } else if (bestFlag) {
        sort_words_by_best_match(words, count);
        print_non_duplicate_words(words, count);
    } else {
        for (int i = 0; i < count; i++) {
            printf("%s\n", words[i]);
        }
    }
    free_memory(words, &count);
}

/* void filter_words_matching_pattern(char patternString[], 
        char dictionaryString[], int *matchesPattern)
* -----------------------------------------------
* Filters words that matches the withString passed from commandline
*
* withoutString: the value of the -with argument
* dictionaryString: the current string from the dictionary that is being 
                    checked
* matchesWith: flag to indicate if the string matches the withString 
*
* Returns: -
* Errors: -
*/
void filter_words_matching_pattern(char patternString[], 
        char dictionaryString[], int *matchesPattern) {
    for (int i = 0; patternString[i]; i++) {
        if (patternString[i] != '_') {
            if (patternString[i] == dictionaryString[i]) {
                *matchesPattern = 1;
            } else {
                *matchesPattern = 0;
                break;
            }
        }
    }
}

/* void filter_words_matching_with(char withString[], char dictionaryString[],
        int *matchesWith)
* -----------------------------------------------
* Filters words that matches the withString passed from commandline
*
* withoutString: the value of the -with argument
* dictionaryString: the current string from the dictionary that is being 
                    checked
* matchesWith: flag to indicate if the string matches the withString 
*
* Returns: -
* Errors: -
*/
void filter_words_matching_with(char withString[], char dictionaryString[],
        int *matchesWith)
{
    for (int j = 0; withString[j]; j++) {
        int withCount = 1;
        int dictCount = 0;
        char withChar = withString[j];
        for (int k = j + 1; withString[k]; k++) {
            if (withChar == withString[k]) {
                withCount++;
            }
        }
        for (int k = 0; dictionaryString[k]; k++) {
            if (dictionaryString[k] == withChar) {
                dictCount++;
            }
        }
        if (dictCount >= withCount) {
            *matchesWith = 1;
        } else {
            *matchesWith = 0;
            break;
        }
    }

}

/* void filter_words_matching_without(char withoutString[], 
        char dictionaryString[], int *matchesWithout)
* -----------------------------------------------
* Filters words that matches the withoutString passed from commandline
*
* withoutString: the value of the -without argument
* dictionaryString: the current string from the dictionary that is being 
                    checked
* matchesWithout: flag to indicate if the string matches the withoutString 
*
* Returns: -
* Errors: -
*/
void filter_words_matching_without(char withoutString[], 
        char dictionaryString[], int *matchesWithout)
{
    for (int j = 0; withoutString[j]; j++) {
        int dictCount = 0;
        char withoutChar = withoutString[j];
        for (int k = 0; dictionaryString[k]; k++) {
            if (dictionaryString[k] == withoutChar) {
                dictCount++;
            }
        }
        if (dictCount > 0) {
            *matchesWithout = 1;
            break;
        } else {
            *matchesWithout = 0;
        }
    }
}

/* int string_comparator(const void *str1, const void *str2)
* -----------------------------------------------
* Compares two strings to check which is greater/lesser in alphabetical order
*
* str1, str2: strings that are to be compared
* 
* Returns: 0, if str1 and str2 are equal
          <0, if str1 is lesser than str2
          >0, if str1 is greater than str2
* Errors: -
*/
int string_comparator(const void *str1, const void *str2)
{
    return strcmp(*(char * const *) str1, *(char * const *) str2);
    // Reference: CSSE2310 C Programming Style Guide Version 2.2.0 [2022]
    // and man pages of qsort()
}

/* int best_guess_comparator(const void *str1, const void *str2)
* -----------------------------------------------
* Compares two strings to check which is greater/lesser according to the
* guess_compare() function to check the likelihood of the occurance of the word
*
* str1, str2: strings that are to be compared
* 
* Returns: 0, if str1 and str2 are equal
          <0, if str1 is lesser than str2
          >0, if str1 is greater than str2
* Errors: -
*/
int best_guess_comparator(const void *str1, const void *str2) 
{
    if ((guess_compare(*(char * const *) str2, *(char * const *) str1))
            == 0) {
        return strcmp(*(char * const *) str1, *(char * const *) str2);
    } else {
        // swap str1, str2 order when compared to string_comparator() as 
        // string_comparator() requires alphabetically sorting(least to most)
        // whereas best_guess_comparator requires ranking by best guess(most to
        // least)
        return guess_compare(*(char * const *) str2, *(char * const *) str1);
    }
}

/* void sort_words_alphabetically(char **words, int count)
* -----------------------------------------------
* Sorts a given array in alphabetical order by using the string_comparator 
* comparison function and qsort()
*
* words: array of words that are to be sorted alphabetically
* count: count of the number of elements in the words array
* 
* Returns: -
* Errors: -
*/
void sort_words_alphabetically(char **words, int count)
{
    qsort(words, count, sizeof(const char*), string_comparator);
}

/* void sort_words_by_best_match(char **words, int count)
* -----------------------------------------------
* Sorts a given array in the best match order using the best_guess_comparator
* comparison function and qsort()
*
* words: array of words that are to be sorted by best match
* count: count of the number of elements in the words array
* 
* Returns: -
* Errors: -
*/
void sort_words_by_best_match(char **words, int count) 
{
    qsort(words, count, sizeof(const char*), best_guess_comparator);
}

/* void print_non_duplicate_words(char **words, int count)
* -----------------------------------------------
* Prints unique words in the array that is passed, 
* skips duplicates
*
* words: array of words that are to be printed to stdout
* count: count of the number of elements in the words array
* 
* Returns: -
* Errors: -
*/
void print_non_duplicate_words(char **words, int count) {
    for (int i = 0; i < count - 1; i++) {
        if (strcmp(words[i], words[i + 1]) != 0) {
            printf("%s\n", words[i]);
        }
    }
    printf("%s\n", words[count - 1]);
}

/* void print_std_err(int value)
* -----------------------------------------------
* Prints out the standard error message on invalid input &
* Exits with the supplied exit code.
*
* value: exit code
* Returns: void
* Errors: -
*/
void print_std_err(int value)
{
    fprintf(stderr, "Usage: wordle-helper [-alpha|-best] [-len len] "
            "[-with letters] [-without letters] [pattern]\n");
    exit(value);
}

/* int check_arg(char *arg)
* -----------------------------------------------
* Checks if the argument that is being parsed is a valid argument or not
*
* arg: the argument that needs to be checked
*
* Returns: 1, if the argument is not a valid argument
           0, if the argument is a valid argument
* Errors: -
*/
int check_arg(char *arg) {
    if ((strcmp(arg, "-len") != 0)
            && (strcmp(arg, "-alpha") != 0)
            && (strcmp(arg, "-best") != 0)
            && (strcmp(arg, "-with") != 0)
            && (strcmp(arg, "-without") != 0)) {
        return 1;
    } else {
        return 0;
    }
}

/* int is_valid_argument(char arg)
* -----------------------------------------------
* Checks if the input argument is valid or not.
*
* arg: commandline argument
* Returns: 1: if valid, 0: if invalid
* Errors: -
*/
int is_valid_argument(char arg)
{
    if (arg == '-') {
        return 1;
    } else {
        return 0;
    }
}

/* void convert_word_to_uppercase(char dictionaryString[], int *foundSpecial)
* -----------------------------------------------
* Checks if the word scanned from the dictionary contains any 
* special characters and converts the word to uppercase if no special
* characters are found
*
* dictionaryString: dictionary string that needs to be converted
* foundSpecial: flag to indicate that a special character is found
*
* Returns: -
* Errors: -
*/
void convert_word_to_uppercase(char dictionaryString[], int *foundSpecial) {
    for (int i = 0; dictionaryString[i]; i++) {
        if (isalpha(dictionaryString[i])) {
            *foundSpecial = 0;
            dictionaryString[i] = toupper(dictionaryString[i]);
        } else {
            *foundSpecial = 1;
            break;
        }
    }
}

/* WordleArguments detect_len_arg(int argc, char *arg, int flag)
* -----------------------------------------------
* Checks validity of -len argument and parses the length value.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that -len argument has been detected,
        0: indicates that -len argument has not been detected.
* Returns: a struct with the updated values of len & lenFlag.
* Errors: exits with code 1, if the argument count < 3 or
*           if the length value is not a number or
            if the length value is < MIN_LEN or > MAX_LEN
*/
WordleArguments detect_len_arg(int argc, char *arg, int flag)
{
    WordleArguments l;
    if (!flag) {
        if (argc < 3) {
            print_std_err(1);
        }
        if (strlen(arg) != 1) {
            print_std_err(1);
        }
        l.len = atoi(arg);
        l.lenFlag = 1;
        if ((l.len > MAX_LEN || l.len < MIN_LEN)) {
            print_std_err(1);
        }
    } else {
        print_std_err(1);
    }
    return l;
}

/* WordleArguments detect_with_arg(int argc, char *arg, int flag)
* -----------------------------------------------
* Checks validity of -with argument and parses the with string.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that -with argument has been detected,
        0: indicates that -with argument has not been detected.
* Returns: a struct with the updated values of withString & withFlag.
* Errors: exits with code 1, if the supplied string contains
          non-alphabet characters or is null
*/
WordleArguments detect_with_arg(int argc, char *arg, int flag)
{
    WordleArguments w;
    if (!flag) {
        if (arg == 0) {
            print_std_err(1);
        }
        if (strlen(arg) <= 0) {
            print_std_err(1);
        }
        for (int i = 0; arg[i]; i++) {
            if (isalpha(arg[i]) == 0) {
                print_std_err(1);
            }
        }
        strcpy(w.withString, arg);
        for (int j = 0; w.withString[j]; j++) {
            w.withString[j] = toupper(w.withString[j]);
        }
        w.withFlag = 1;
    } else {
        print_std_err(1);
    }
    return w;
}

/* WordleArguments detect_without_arg(int argc, char *arg, int flag)
* -----------------------------------------------
* Checks validity of -without argument and parses the without string.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that -without argument has been detected,
        0: indicates that -without argument has not been detected.
* Returns: a struct with the updated values of withoutString & withoutFlag.
* Errors: exits with code 1, if the supplied string contains
          non-alphabet characters or is null
*/
WordleArguments detect_without_arg(int argc, char *arg, int flag)
{
    WordleArguments wo;
    if (!flag) {
        if (arg == 0) {
            print_std_err(1);
        }
        if (strlen(arg) <= 0) {
            print_std_err(1);
        }
        for (int i = 0; arg[i]; i++) {
            if (isalpha(arg[i]) == 0) {
                print_std_err(1);
            }
        }
        strcpy(wo.withoutString, arg);
        for (int j = 0; wo.withoutString[j]; j++) {
            wo.withoutString[j] = toupper(wo.withoutString[j]);
        }
        wo.withoutFlag = 1;
    } else {
        print_std_err(1);
    }
    return wo;
}

/* WordleArguments detect_alpha_arg(int argc, char *arg, int alphaFlag,
*        int bestFlag)
* -----------------------------------------------
* Checks validity of -alpha argument and sets the alphaFlag.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that -alpha argument has been detected,
        0: indicates that -alpha argument has not been detected.
* Returns: a struct with the updated values of alphaFlag.
* Errors: exits with code 1, if the -best arg has been supplied along with the
          -alpha arg or if the -alpha arg has been supplied more than once
*/
WordleArguments detect_alpha_arg(int argc, char *arg, int alphaFlag,
        int bestFlag)
{
    WordleArguments a;
    if (alphaFlag == 1) {
        print_std_err(1);
    }
    if (bestFlag == 1) {
        print_std_err(1);
    }
    a.alphaFlag = 1;
    return a;
}

/* WordleArguments detect_best_arg(int argc, char *arg, int bestFlag,
*        int alphaFlag)
* -----------------------------------------------
* Checks validity of -best argument and sets the bestFlag.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that -best argument has been detected,
        0: indicates that -best argument has not been detected.
* Returns: a struct with the updated values of bestFlag.
* Errors: exits with code 1, if the -best arg has been supplied along with the
          -alpha arg or if the -best arg has been supplied more than once
*/
WordleArguments detect_best_arg(int argc, char *arg,
        int bestFlag, int alphaFlag)
{
    WordleArguments b;
    if (bestFlag == 1) {
        print_std_err(1);
    }
    if (alphaFlag == 1) {
        print_std_err(1);
    }
    b.bestFlag = 1;
    return b;
}

/* WordleArguments detect_pattern_arg(int argc, char *arg, int patternFlag,
*        int len)
* -----------------------------------------------
* Checks validity of the supplied pattern and parses the pattern.
*
* argc: count of commandline arguments.
* arg: the current commandline argument that is being checked.
* flag: 1: indicates that a pattern string argument has been detected,
        0: indicates that a pattern string argument has not been detected.
* Returns: a struct with the updated values of patternFlag & patternString.
* Errors: exits with code 2, if the supplied pattern is longer than
*         the -len value or with code 1 if a pattern has been supplied
*         more than once
*/
WordleArguments detect_pattern_arg(int argc, char *arg, int flag, int len)
{
    WordleArguments p;
    if (flag == 1) {
        print_std_err(1);
    }
    strcpy(p.patternString, arg);
    for (int i = 0; p.patternString[i]; i++) {
        if (isalpha(p.patternString[i]) == 0 && p.patternString[i] != '_') {
            fprintf(stderr,
                    "wordle-helper: pattern must be of length %d and "
                    "only contain underscores and/or letters\n", len);
            exit(2);
        }
    }
    for (int j = 0; p.patternString[j]; j++) {
        p.patternString[j] = toupper(p.patternString[j]);
    }
    if (strlen(p.patternString) != len) {
        fprintf(stderr,
                "wordle-helper: pattern must be of length %d and "
                "only contain underscores and/or letters\n", len);
        exit(2);
    }
    p.patternFlag = 1;
    return p;
}

/* FILE *open_dictionary()
* -----------------------------------------------
* Checks for the existence of the dictionary file and opens it.
* If the WORDLE_DICTIONARY environment variable is set, the dictionary file is
* loaded from there, else defaults to "/usr/share/dict/words"

* Returns: a file object with the parsed words from the dictionary file
* Errors: exits with code 3, if the dictionary file cannot be accessed or
*         if the dictionary file is empty
*/
FILE *open_dictionary()
{
    FILE *dictionary;
    if (getenv("WORDLE_DICTIONARY")) {
        dictionary = fopen(getenv("WORDLE_DICTIONARY"), "r");
        if (dictionary == 0) {
            fprintf(stderr,
                    "wordle-helper: dictionary file \"%s\" cannot be opened\n",
                    getenv("WORDLE_DICTIONARY"));
            exit(3);
        }
    } else {
        dictionary = fopen("/usr/share/dict/words", "r");
        if (dictionary == 0) {
            fprintf(stderr,
                    "wordle-helper: dictionary file \"/usr/share/dict/words\" "
                    "cannot be opened\n");
            exit(3);
        }
    }
    return dictionary;
}

/* void set_default_len(int *lenFlag, int *len)
* -----------------------------------------------
* If length is not specified, sets length to the DEFAULT_LEN value
*
* lenFlag: flag that indicates if length has been specified or not
* len: length that is set to DEFAULT_LEN value
* 
* Returns:-
* Errors: -
*/
void set_default_len(int *lenFlag, int *len)
{
    if (*lenFlag == 0) { 
        *len = DEFAULT_LEN; 
    }
}

/* void free_memory(char **words, int *count)
* -----------------------------------------------
* Free the memory allocated for storing words
*
* words: array of strings that is to be freed
* count: number of strings is the array
*
* Returns: -
* Errors: -
*/
void free_memory(char **words, int *count) 
{
    for (int i = 0; i < *count; i++) {
        free(words[i]);
    }
    free(words);
}
