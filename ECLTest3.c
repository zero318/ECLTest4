#include "SHUT_UP_VS.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <ctype.h>

#include "ECLTest3.h"
#include "ECL_util.h"
#include "Tokenizer.h"
#include "ArgParsing.h"
#include "SquitLib/SquitUtil.h"
#include "SquitLib/SquitStrings.h"

//#include "_RegisterWatch.h"
#include "__TEST.h"

//static char* Filepath;
//static const char (*const Options)[] = {"?", ""};
//static const char* const Options[] = { "?", "", "yeet" };
//static const char OptionsSize = sizeof(Options) / sizeof(char*);

//void ParseArgs(int ArgCount, char* Args[], const char* const AcceptedArgs) {
//	Filepath = Args[ArgCount--];
//	while (ArgCount-- > 0) {
//		char* temp = Args[ArgCount];
//		char temp2 = temp[0];
//		switch (temp2) {
//			case '\0':
//			case '-':
//				
//			default:
//				continue;
//		}
//	}
//}

noreturn inline void PrintExit(const char *const ExitMessage) {
    fprintf(stdout, "%s", ExitMessage);
    exit(EXIT_SUCCESS);
}

noreturn inline void PrintHelp() {
    PrintExit("New ECL Compiler Test 0.1\n"
              " -? -v Prints this help message and version info\n"
              " -d #  Dump mode and game version\n"
              " -c #  Compile mode and game version\n"
              " -I    Input file\n"
              " -O    Output file\n"
              " -M    Mapping file\n"
              " -S    Symbol file\n"
              " -f    Force overwrite mode\n"
              " -e    Character Encoding (default ANSI)\n"
              );
}

noreturn inline void PrintErrorExit(const char *const ErrorMessage) {
    fprintf(stderr, "%s", ErrorMessage);
    exit(EXIT_FAILURE);
}

//This is just my best attempt at testing whether a file exists
inline bool FileTest(const char* Filename) {
    register FILE* temp = fopen(Filename, "r");
    if (!temp) {
        return false;
    }
    fclose(temp);
    return true;
}

typedef struct Args Args;
struct Args {
    uint32_t Compile : 1;
    uint32_t Dump : 1;
    uint32_t Help : 1;
    uint32_t MapFile : 1;
    uint32_t InFile : 1;
    uint32_t OutFile : 1;
    uint32_t SymbolFile : 1;
    uint32_t Overwrite : 1;
    uint32_t Encoding : 8;
    uint32_t Version : 16;
    char* InFilename;
    char* OutFilename;
    char* MapFilename;
    char* SymbolFilename;
};
//typedef enum Arg {None, Compile, Dump, Help, Version, MapFile } Arg;

static FILE* InputFile = NULL;
static FILE* OutputFile = NULL;
static FILE* MapFile = NULL;
static FILE* SymbolFile = NULL;

//IDK if this even needs to be here tbh
void CloseFilesAtExit() {
    if (InputFile) fclose(InputFile);
    if (OutputFile) fclose(OutputFile);
    if (MapFile) fclose(MapFile);
    if (SymbolFile) fclose(SymbolFile);
}


int main(int argc, char* argv[]) {
    
    main_thing(argc, argv);

#pragma region Command line argument parsing
/*
* Command Line Argument Parsing
*   This block is trying to parse whatever command line arguments into
*   something meaningful. It's kind of spaghet though and really shouldn't
*   be dumped at the top of main.
* 
*   TODO: Move to ArgParsing.c
*   TODO: Succ less
*/
    if (argc <= 1) {
        PrintHelp();
    }
    Args ArgsData;
    atexit(&CloseFilesAtExit);
    InputFile = OutputFile = MapFile = NULL;
    memset(&ArgsData, 0, sizeof(Args));
    enum { Option, Version, File, Encoding } NextArg = Option;
    enum { Map, Input, Output, Symbol } NextFile;
    int ArgCount = argc;
    int i = 0;
    size_t j;
    while (++i < ArgCount) {
        if (!strlen(argv[i])) {
            continue;
        }
        switch (NextArg) {
            case Option:
                if (argv[i][0] != '-' || strlen(argv[i]) == 1) {
                    continue;
                }
                switch (argv[i][1]) {
                    //Mode Options
                    case '?': case 'v': case 'V':
                        ArgsData.Help = true;
                        continue;
                    case 'd': case 'D':
                        ArgsData.Dump = true;
                        NextArg = Version;
                        continue;
                    case 'c': case 'C':
                        ArgsData.Compile = true;
                        NextArg = Version;
                        continue;
                    //File I/O Options
                    case 'm': case 'M':
                        NextArg = File;
                        NextFile = Map;
                        continue;
                    case 'i': case 'I':
                        NextArg = File;
                        NextFile = Input;
                        continue;
                    case 'o': case 'O':
                        NextArg = File;
                        NextFile = Output;
                        continue;
                    case 's': case 'S':
                        NextArg = File;
                        NextFile = Symbol;
                        continue;
                    //Behavior Options
                    case 'f': case 'F':
                        ArgsData.Overwrite = true;
                        continue;
                    case 'e': case 'E':
                        NextArg = Encoding;
                        continue;
                    default:
                        //ArgsData.WTF = true;
                        printf("Unrecognized option: %s", argv[i]);
                        continue;
                }
            case Version:
                NextArg = Option;
                j = strlen(argv[i]);
                bool AllNumeric = true;
                while (j > 0 && AllNumeric) {
                    AllNumeric = (bool)isdigit(argv[i][--j]);
                }
                if (AllNumeric) {
                    ArgsData.Version = (uint16_t)atoi(argv[i]);
                    continue;
                }
                continue;
            case File:
                NextArg = Option;
                if (NextFile == Output) {
                    ArgsData.OutFile = true;
                    ArgsData.OutFilename = argv[i];
                    continue;
                }
                if (!FileTest(argv[i])) {
                    continue;
                }
                switch (NextFile) {
                    case Input:
                        ArgsData.InFile = true;
                        ArgsData.InFilename = argv[i];
                        continue;
                    case Map:
                        ArgsData.MapFile = true;
                        ArgsData.MapFilename = argv[i];
                        continue;
                    case Symbol:
                        ArgsData.SymbolFile = true;
                        ArgsData.SymbolFilename = argv[i];
                        continue;
                    default:
                        continue;
                }
            case Encoding:
                NextArg = Option;
                size_t Length = strlen(argv[i]) + 1;
                j = Length;
                //size_t k = 0;
                char* EncodingTemp = callocNonNull(Length, sizeof(char));
                while (j > 0) {
                    --j;
                    EncodingTemp[j] = toupper(argv[i][j]);
                }
                //j and k will always be 0 when reaching here
                /*while (j < Length && EncodingTemp[j] != '\0') {
                    if (EncodingTemp[j] == '-') {
                        memmove(&EncodingTemp[k], &EncodingTemp[j+1], Length - j);
                    } else {
                        ++j;
                        ++k;
                    }
                }*/
                char* EncodingTemp2 = StringDupe(EncodingTemp);
                char* EncodingTemp3 = StringDupe(EncodingTemp);
                //StringCStrip3(EncodingTemp, '-');
                //StringCStripFast(EncodingTemp2, '-');
                //StringCStripReplaceFast(EncodingTemp3, '-', 'e');
                StringSStrip(EncodingTemp, "-?");
                StringSStrip(EncodingTemp2, "-?");
                StringSStripReplace(EncodingTemp3, "-?", 'e');
                if (strcmp(EncodingTemp, "ANSI") == 0 ) {
                    ArgsData.Encoding = TokenANSI;
                } else if (strcmp(EncodingTemp, "UTF8") == 0) {
                    ArgsData.Encoding = TokenUTF8;
                } else if (strcmp(EncodingTemp, "UTF16") == 0) {
                    ArgsData.Encoding = TokenUTF16;
                } else if (strcmp(EncodingTemp, "UTF32") == 0) {
                    ArgsData.Encoding = TokenUTF32;
                } else if (strcmp(EncodingTemp, "UCS2") == 0) {
                    ArgsData.Encoding = TokenUCS2;
                } else if (strcmp(EncodingTemp, "UCS4") == 0) {
                    ArgsData.Encoding = TokenUCS4;
                } else if (strcmp(EncodingTemp, "SHIFTJIS") == 0 || strcmp(EncodingTemp, "SJIS") == 0) {
                    ArgsData.Encoding = TokenShiftJIS;
                } else if (strcmp(EncodingTemp, "UNICODE") == 0) {
                    //Let's be real, when someone says "unicode"
                    //they probably expect it to mean UTF8.
                    ArgsData.Encoding = TokenUTF8;
                } else {
                    //Don't do this plz
                    ArgsData.Encoding = TokenDefault;
                }
                free(EncodingTemp);
                continue;
        }
    }
    if (ArgsData.Help) {
        PrintHelp();
    }
    switch (ArgsData.Version) {
        case 0:
            PrintErrorExit("No version specified!");
        default:
            PrintErrorExit("Unsupported version!");
        case 6: case 7: case 8: case 9: case 95:
            break;
    }
    if (ArgsData.Compile & ArgsData.Dump) {
        if (ArgsData.Compile && ArgsData.Dump) {
            PrintErrorExit("Compile and dump are incompatible!");
        }
        PrintErrorExit("Must select either compile or dump!");
    }
    if (!ArgsData.InFile) {
        PrintErrorExit("No input file specified!\n");
    } else if (!ArgsData.MapFile) {
        PrintErrorExit("No map file specified!\n");
    } else if (!ArgsData.OutFile) {
        PrintErrorExit("No output file specified!\n");
    }
    if (!ArgsData.Overwrite && FileTest(ArgsData.OutFilename)) {
        PrintErrorExit("Output file already exists!\n"
                       "To enable overwrite, use -f option.\n");
    }
#pragma endregion
/*
* The actual functional part of the crap
*   This part of the code should only be reached after all
*   arguments have been validated.
*   
*   TODO: Finish questioning whether this is the right place to put the fopen stuff
*/
    OutputFile = fopen(ArgsData.OutFilename, "w");
    InputFile = fopen(ArgsData.InFilename, "r");
    MapFile = fopen(ArgsData.MapFilename, "r");
    if (ArgsData.SymbolFile) { 
        SymbolFile = fopen(ArgsData.SymbolFilename, "r");
    }
    if (ArgsData.Compile) {
        //I'm well aware that this is passing the filename string
        //instead of the FILE pointer. It's a feature, okay?
        tokenize(ArgsData.InFilename, Compile);
    }
    return EXIT_SUCCESS;
}