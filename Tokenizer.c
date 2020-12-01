#include "SHUT_UP_VS.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdnoreturn.h>
#include <wchar.h>
#include <wctype.h>
#include <uchar.h>

#include "Tokenizer.h"
#include "SquitLib/SquitObjects.h"
#include "SquitLib/SquitUtil.h"
#include "SquitLib/SquitStrings.h"
#include "SquitLib/SquitTypes.h"
#include "SquitLib/SquitIO.h"
#include "StringStream.h"

/*
* Alright, so this file is currently a mess.
* 
* Thing is, it's currently a horrible frankenstein of two separate tokenizer
* versions stuck together, both of which are spaghet awfulness produced by
* yours truly.
* 
* The first version is down near the bottom and used direct input from
* a FILE* to produce a linked list of tokens. Admittedly it worked, but wasn't
* going to keep working after attaching a compiler to it.
* 
* The second version is attempting to follow the "proper" C syntax and is
* generally better code, but it isn't finished and definitely doesn't work
* yet.
*/

#pragma region Struct defs
DeclareStruct(ECLDefine);
struct ECLDefine {
    char* name;
    char* value;
    ECLDefine* next;
    ECLDefine* previous;
};
DefineConstructor(ECLDefine, .name = NULL, .value = NULL, .next = NULL, .previous = NULL);

DeclareStruct(ECLToken);
struct ECLToken {
    char* value;
    ECLToken* next;
    ECLToken* previous;
};
DefineConstructor(ECLToken, .value = NULL, .next = NULL, .previous = NULL);
#pragma endregion

#define CommentToken '\\'
#define BlankLine '\n'

#define PreprocessorStarter '#'
#define PreprocessorEndToken ' '
#define PreprocessorContinue '\\'
#define PreprocessorEnd '\n'

#define MainProcessorEndToken ' '
#define MainProcessorEnd ';'

#define NotImplemented(text) printf(#text"isn't implemented yet!")

//Yes, this uses a comma operator. It's just to maintain
//internal state or some crap though, so whatever.
#define StringStreamNextChar(SS) (SS.CurrentPos++, SS.String[SS.Pos++])

void FreeDefines(ECLDefine* Defines) {
    ECLDefine* DefinesLast;
    ECLDefine* DefinesAccess = Defines;
    while (DefinesAccess != NULL) {
        DefinesLast = DefinesAccess;
        DefinesAccess = DefinesAccess->next;
        free(DefinesLast->name);
        free(DefinesLast->value);
        free(DefinesLast);
    }
}
void FreeTokens(ECLToken* Tokens) {
    ECLToken* TokensLast;
    ECLToken* TokensAccess = Tokens;
    while (TokensAccess != NULL) {
        TokensLast = TokensAccess;
        TokensAccess = TokensAccess->next;
        free(TokensLast->value);
        free(TokensLast);
    }
}

char* MainProcessorReadText(FILE* File, char** Buffer) {
    if (*Buffer != NULL) {
        free(*Buffer);
    }
    size_t Counter = 0;
    size_t Length;
    fpos_t PosTemp;
    char temp;
    fgetpos(File, &PosTemp);
    do {
        temp = fgetc(File);
        if (feof(File)) {
            return NULL;
        }
        if (temp == MainProcessorEnd) {
            break;
        }
        ++Counter;
    } while (1);
    fsetpos(File, &PosTemp);
    if (Counter > 0) {
        Length = Counter + 1;
        *Buffer = callocNonNull(Length, sizeof(char));
        Length -= 2;
        while (Counter-- > 0) {
            (*Buffer)[Length - Counter] = fgetc(File);
        }
        fgetc(File);
    } else {
        *Buffer = NULL;
    }
    return *Buffer;
}

void MainProcessorTokenize(char** Buffer, ECLDefine** Defines, ECLToken** Tokens);
void MainProcessorTokenize(char** Buffer, ECLDefine** Defines, ECLToken** Tokens) {

    ECLDefine** DefinesAccess;

    ECLToken* TokensLast;
    ECLToken** TokensAccess;
    ECLToken* NextToken;

    if (*Buffer == NULL) {
        return;
    }
    char* original_string = StringDupe(*Buffer);
    char* substring;
    ptrdiff_t substring_index;
    ptrdiff_t previous_index;
    char* token;
    flag first_call = true;
    size_t Counter = 0;
    flag define_match;
    do {
        token = strtok(first_call ? first_call = false, *Buffer : NULL, " (){},[];\n\t");
        if (token == NULL) {
            break;
        }
        substring = strstr((char*)&(original_string[Counter]), token);
        substring_index = (substring - original_string) - 1;
        previous_index = substring_index - Counter;
        if (previous_index > -1) {
            TokensAccess = Tokens;
            TokensLast = NULL;
            while (*TokensAccess != NULL) {
                TokensLast = *TokensAccess;
                TokensAccess = &(*TokensAccess)->next;
            }
            while (previous_index-- > -1) {
                switch (original_string[Counter]) {
                    case '(':case ')':case '[':case ']':case '{':case '}':case ';':case ',':
                        NextToken = NewECLToken();
                        TokensLast->next = NextToken;
                        NextToken->previous = TokensLast;
                        NextToken->value = StringDupe((char[]) {
                            original_string[Counter++], '\0'
                        });
                        TokensLast = NextToken;
                        continue;
                    default:
                        ++Counter;
                        continue;
                }
            }
        }
        Counter += strlen(token);
        DefinesAccess = Defines;
        define_match = false;
        while (*DefinesAccess != NULL) {
            if (strcmp(token, (*DefinesAccess)->name) == 0) {
                define_match = true;
                break;
            }
            DefinesAccess = &(*DefinesAccess)->next;
        }
        if (define_match) {
            MainProcessorTokenize(&(*DefinesAccess)->value, Defines, Tokens);
        } else {
            TokensAccess = Tokens;
            TokensLast = NULL;
            while (*TokensAccess != NULL) {
                TokensLast = *TokensAccess;
                TokensAccess = &((*TokensAccess)->next);
            }
            NextToken = NewECLToken();
            *TokensAccess = NextToken;
#pragma warning(suppress: 28182)
            NextToken->previous = TokensLast;
            NextToken->value = StringDupe(token);
        }
        switch (original_string[Counter]) {
            case '(':case ')':case '[':case ']':case '{':case '}':case ';':case ',':
                TokensAccess = Tokens;
                TokensLast = NULL;
                while (*TokensAccess != NULL) {
                    TokensLast = *TokensAccess;
                    TokensAccess = &(*TokensAccess)->next;
                }
                NextToken = NewECLToken();
                *TokensAccess = NextToken;
                NextToken->previous = TokensLast;
                NextToken->value = StringDupe((char[]) {
                    original_string[Counter++], '\0'
                });
                continue;
            default:
                ++Counter;
                continue;
        }
    } while (1);
    free(original_string);
    return;
}

char* PreprocessorReadText(FILE* File, char** Buffer) {
    if (*Buffer != NULL) {
        free(*Buffer);
    }
    size_t Counter = 0;
    size_t Length;
    fpos_t PosTemp;
    char temp;
    flag HasContinueToken = false;
    fgetpos(File, &PosTemp);
    do {
        temp = fgetc(File);
        if (feof(File)) {
            break;
        }
        switch (temp) {
            case PreprocessorEndToken:
                //HasContinueToken = false;
                goto BreakPreprocessorLoop;
            //case PreprocessorContinue:
            //    //
            //    //This isn't quite working right, but whatever. Will fix later
            //    //
            //    HasContinueToken = true;
            //    continue;
            case PreprocessorEnd:
               // if (!HasContinueToken) {
                    goto BreakPreprocessorLoop;
                //}
                //continue;
            default:
                ++Counter;
                continue;
        }
    } while (1);
BreakPreprocessorLoop:
    fsetpos(File, &PosTemp);
    if (Counter > 0) {
        Length = Counter + 1;
        *Buffer = callocNonNull(Length, sizeof(char));
        Length -= 2;
        while (Counter-- > 0) {
            (*Buffer)[Length - Counter] = fgetc(File);
        }
        fgetc(File);
    } else {
        *Buffer = NULL;
    }
    return *Buffer;
}

int parse_math() {
    return 0;
}

//
//WIP:
//The token parsing junk should probably be moved into a separate file.
//Then something like TokenFind(Tokens, "TokenName") could simplify this mess.
//

int ProcessTrigraph(FILE* File) {
    fpos_t PosRestore;
    char TempBuffer[] = {'\0','\0','\0'};
    fgetpos(File, &PosRestore);
    fscanf(File, "%2c", TempBuffer);
    if (feof(File) || ferror(File)) {
        fsetpos(File, &PosRestore);
        return '\?';
    } else if (strcmp(TempBuffer, "\?<") == 0) {
        return ungetc('{', File);
    } else if (strcmp(TempBuffer, "\?>") == 0) {
        return ungetc('}', File);
    } else if (strcmp(TempBuffer, "\?(") == 0) {
        return ungetc('[', File);
    } else if (strcmp(TempBuffer, "\?)") == 0) {
        return ungetc(']', File);
    } else if (strcmp(TempBuffer, "\?=") == 0) {
        return ungetc('#', File);
    } else if (strcmp(TempBuffer, "\?/") == 0) {
        return ungetc('\\', File);
    } else if (strcmp(TempBuffer, "\?\'") == 0) {
        return ungetc('^', File);
    } else if (strcmp(TempBuffer, "\?!") == 0) {
        return ungetc('|', File);
    } else if (strcmp(TempBuffer, "\?-") == 0) {
        return ungetc('~', File);
    } else {
        fsetpos(File, &PosRestore);
        return '\?';
    }
}

bool NewlineCheck(FILE* __restrict File) {
    return false;
}

bool ProcessCR(FILE* __restrict File) {
    fpos_t PosRestore;
    fgetpos(File, &PosRestore);
    char temp = fgetc(File);
    if (feof(File) || ferror(File)) {
        fsetpos(File, &PosRestore);
        return false;
    } else if (temp != '\n') {
        ungetc(temp, File);
    }
    ungetc('\n', File);
    return true;
}

bool ProcessEscape(FILE* File) {
    fpos_t PosRestore;
    char TempBuffer[3] = {'\0', '\0', '\0'};
    fgetpos(File, &PosRestore);
    ungetc('\\', File);
    fscanf(File, "%2c", &TempBuffer[0]);
    if (feof(File) || ferror(File)) {
        fsetpos(File, &PosRestore);
        return false;
    } else if (strcmp(&TempBuffer[0], "\\a") == 0) {
        ungetc('\a', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\b") == 0) {
        ungetc('\b', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\f") == 0) {
        ungetc('\f', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\n") == 0) {
        ungetc('\n', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\r") == 0) {
        ungetc('\r', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\t") == 0) {
        ungetc('\t', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\v") == 0) {
        ungetc('\v', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\\\") == 0) {
        ungetc('\\', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\\'") == 0) {
        ungetc('\'', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\\"") == 0) {
        ungetc('\"', File);
        return true;
    } else if (strcmp(&TempBuffer[0], "\\\?") == 0) {
        ungetc('\?', File);
        return true;
    } else {
        fsetpos(File, &PosRestore);
        return false;
    }
    return false;
}

bool ProcessString(FILE* File) {
    return false;
}

bool ProcessCharacter(FILE* File) {
    return false;
}

bool ProcessPreprocessor(FILE* File) {
    return false;
}

static ECLDefine* Defines = NULL;
static ECLToken* Tokens = NULL;

typedef struct SourceToken SourceToken;
struct SourceToken {
    char* Token;
    size_t Length;
    enum {
        Unknown,
        Keyword,
        Identifier,
        Constant,
        StringToken,
        Operator,
        Punctuator
    } TokenType;
    SourceToken* Next;
    size_t Line;
};

typedef unsigned char mbchar_array_t[MB_LEN_MAX];
static const size_t TokenSizes[7] = {
    /*TokenChar8*/      sizeof(char),
    /*TokenChar16*/     sizeof(char16_t),
    /*TokenChar32*/     sizeof(char32_t),
    /*TokenWChar*/      sizeof(wchar_t),
    /*TokenWInt*/       sizeof(wint_t),
    /*TokenMBChar*/     sizeof(mbchar_array_t),
    /*TokenInt*/        sizeof(int)
};
enum TokenSize {
    TokenChar8 = 0,     //TokenSizes[0]
    TokenChar16 = 1,    //TokenSizes[1]
    TokenChar32 = 2,    //TokenSizes[2]
    TokenWChar = 3,     //TokenSizes[3]
    TokenWInt = 4,      //TokenSizes[4]
    TokenMBChar = 5,    //TokenSizes[5]
    TokenInt = 6        //TokenSizes[6]
};
static const int TokenEncodingSizes[8] = {
    /*TokenDefault*/    TokenChar8,
    /*TokenANSI*/       TokenChar8,
    /*TokenUTF8*/       TokenChar8,
    /*TokenUTF16*/      TokenChar16,
    /*TokenUTF32*/      TokenChar32,
    /*TokenUCS2*/       TokenChar16,
    /*TokenUCS4*/       TokenChar32,
    /*TokenShiftJIS*/   TokenMBChar
};
//static enum TokenEncoding {
//    TokenDefault = 0,   //TokenEncodingSizes[0]
//    TokenANSI = 1,      //TokenEncodingSizes[1]
//    TokenUTF8 = 2,      //TokenEncodingSizes[2]
//    TokenUTF16 = 3,     //TokenEncodingSizes[3]
//    TokenUTF32 = 4,     //TokenEncodingSizes[4]
//    TokenUCS2 = 5,      //TokenEncodingSizes[5]
//    TokenUCS4 = 6,      //TokenEncodingSizes[6]
//    TokenShiftJIS = 7   //TokenEncodingSizes[7]
//};

typedef struct CToken {
    struct CTokenBase {
        uint_fast8_t Encoding;
        uint_fast8_t EncodingSize;
        size_t Length;
    };
    union {
        struct CTokenChar8Payload {
            char* String;
        } Char8;
        struct CTokenChar16Payload {
            char16_t* String;
        } Char16;
        struct CTokenChar32Payload {
            char32_t* String;
        } Char32;
        struct CTokenWCharPayload {
            wchar_t* String;
        } WChar;
        struct CTokenMBCharPayload {
            mbchar_array_t* String;
            size_t Bytes;
        } MBChar;
    };
} CToken;

typedef struct SourceFile SourceFile;
struct SourceFile {
    SourceToken* Tokens;
    SourceToken** NextToken;
    char* Filename;
    char* TrueFilename;
    SourceFile* Next;
};
static SourceFile* Source = NULL;
static SourceFile** NextSourceFile = &Source;

enum {
    MainProcessor = -2,

    Trigraph = '\?',
    Escape = '\\',


    String = '\"',
    Character = '\'',

    Comment = '/', //TokenizerMode
    BlockComment = '*',
    LineComment = '/',
    Preprocessor = '#', //TokenizerMode
    Space = ' ',
    HorizontalTab = '\t',
    Newline = '\n',
    VerticalTab = '\v',
    FormFeed = '\f'
};

const char* const OperatorList[] = {
    "++",
    "--",
    "->"
    "*",
    "/",
    "%"

};

void FreeSourceTokens(SourceToken* Tokens) {
    SourceToken* TokenLast;
    while (Tokens != NULL) {
        TokenLast = Tokens;
        Tokens = Tokens->Next;
        if (TokenLast->Token != NULL) free(TokenLast->Token);
        free(TokenLast);
    }
}

//inline SourceToken* AddSourceToken(SourceFile* Source) {
//    register SourceToken* temp;
//    *(Source->NextToken) = mallocNonNull(sizeof(SourceToken));
//    temp = *(Source->NextToken);
//    Source->NextToken = &(temp->Next);
//    *temp = (SourceToken){ .Length = 0, .TokenType = Unknown, .Next = NULL, .Token = NULL };
//    return temp;
//}

inline SourceToken* FinalizeToken(SourceToken* Token, char* TokenSource) {
    Token->Token = mallocNonNull(Token->Length * sizeof(char));
    strcpy(Token->Token, TokenSource);
    return Token;
}

inline void AppendToken(SourceFile* Source, SourceToken* Token) {
    register SourceToken* temp;
    temp = *(Source->NextToken) = (SourceToken*)mallocNonNull(sizeof(SourceToken));
    Source->NextToken = &temp->Next;
    *temp = *Token;
}

void FreeSourceFiles(SourceFile* Source) {
    SourceFile* SourceLast;
    while (Source != NULL) {
        SourceLast = Source;
        Source = Source->Next;
        if (SourceLast->Filename != NULL) free(SourceLast->Filename);
        if (SourceLast->Tokens != NULL) FreeSourceTokens(SourceLast->Tokens);
        free(SourceLast);
    }
}

inline bool isspace_tokenize(char c) {
    switch (c) {
        case Space: case HorizontalTab: case VerticalTab: case FormFeed:
            return true;
        default:
            return false;
    }
}

inline bool isspace_ornull(char c) {
    switch (c) {
        case Space: case HorizontalTab: case VerticalTab: case FormFeed: case Newline: case '\r': case '\0':
            return true;
        default:
            return false;
    }
}

static const char DefineString[] = "define";
static const char UnDefString[] = "undef";
static const char IncludeString[] = "include";
static const char IfString[] = "if";
static const char ElseString[] = "else";
static const char ElIfString[] = "elif";
static const char EndIfString[] = "endif";
static const char IfDefString[] = "ifdef";
static const char IfNDefString[] = "ifndef";
static const char PragmaString[] = "pragma";
static const char ErrorString[] = "error";
static const char LineString[] = "line";
static const char UnknownDirectiveString[] = "unknown directive";
static const char* PreprocessorStrings[] = {
    DefineString,
    UnDefString,
    IncludeString,
    IfString,
    ElseString,
    ElIfString,
    EndIfString,
    IfDefString,
    IfNDefString,
    PragmaString,
    ErrorString,
    LineString,
    UnknownDirectiveString
};
typedef enum DirectiveType {
    Define = 0,
    UnDef = 1,
    Include = 2,
    If = 3,
    Else = 4,
    ElIf = 5,
    EndIf = 6,
    IfDef = 7,
    IfNDef = 8,
    Pragma = 9,
    Error = 10,
    Line = 11,
    UnknownDirective = 12
} DirectiveType;

noreturn inline void PrintErrorLineInfoExit(const char* ErrorType, char* Token, size_t LineNumber, char* Filename) {
    fprintf(stderr, "Error in %s \"%s\" on line %d of file \"%s\".\n", ErrorType, Token, LineNumber, Filename);
    exit(EXIT_FAILURE);
}

//CRAP, what do you mean there can be
//spaces between the # and the directive?
void RunPreprocessor(char* Token, size_t* LineNumber, char** Filename, char* TrueFilename) {
    char* OriginalToken = StringDupe(Token);
    //The newline is included at the end,
    //so length will always be at least 1
    size_t Length = strlen(Token) - 1;
    if (Length == 0) {
        return;
    }
    bool EverythingIsBurning = false;
    size_t i = 0;
    //Skip spaces after #
    while (isspace_tokenize(Token[i])) {
        //If nothing is found before the end of the string,
        //that means it's a null directive and can be ignored.
        if (++i == Length) return;
    }
    //Mark where the directive starts
    char* DirectiveString = &Token[i];
    //Scan the whole directive string
    while (!isspace(Token[i])) {
        ++i;
    }

    /*
    * HERE BE DRAGONS
    */

    Token[i++] = '\0';
    char* TokenArgs = &Token[i];
    DirectiveType DirectiveType;
    size_t MinArgCount, MaxArgCount;
    char* ArgDelims = " \n";
    //Just determine the directive type for now since there could still
    //be a parsing error before the end of the line.
    if (strcmp(DirectiveString, DefineString) == 0) {
        DirectiveType = Define;
        MinArgCount = 1;
        MaxArgCount = SIZE_MAX;
        ArgDelims = " \n()";
    } else if (strcmp(DirectiveString, IncludeString) == 0) {
        DirectiveType = Include;
        MinArgCount = 1;
        MaxArgCount = 1;
    } else if (strcmp(DirectiveString, IfDefString) == 0) {
        DirectiveType = IfDef;
        MinArgCount = 1;
        MaxArgCount = 1;
    } else if (strcmp(DirectiveString, IfNDefString) == 0) {
        DirectiveType = IfNDef;
        MinArgCount = 1;
        MaxArgCount = 1;
    } else if (strcmp(DirectiveString, IfString) == 0) {
        DirectiveType = If;
        MinArgCount = 1;
        MaxArgCount = SIZE_MAX;
    } else if (strcmp(DirectiveString, ElIfString) == 0) {
        DirectiveType = ElIf;
        MinArgCount = 1;
        MaxArgCount = SIZE_MAX;
    } else if (strcmp(DirectiveString, ElseString) == 0) {
        DirectiveType = Else;
        MinArgCount = 0;
        MaxArgCount = 0;
    } else if (strcmp(DirectiveString, EndIfString) == 0) {
        DirectiveType = EndIf;
        MinArgCount = 0;
        MaxArgCount = 0;
    } else if (strcmp(DirectiveString, PragmaString) == 0) {
        DirectiveType = Pragma;
        MinArgCount = 0;
        MaxArgCount = SIZE_MAX;
    } else if (strcmp(DirectiveString, ErrorString) == 0) {
        DirectiveType = Error;
        MinArgCount = 1;
        MaxArgCount = 1;
    } else if (strcmp(DirectiveString, LineString) == 0) {
        DirectiveType = Line;
        MinArgCount = 1;
        MaxArgCount = 2;
    } else {
        DirectiveType = UnknownDirective;
        EverythingIsBurning = true;
        PrintErrorLineInfoExit(UnknownDirectiveString, DirectiveString, *LineNumber, *Filename);
    }
//    //Token[i++] = ' ';
//    
//    size_t ArgCount = 0;
//    //Count the args.
//    while (ArgCount < MaxArgCount) {
//        //char temp = ;
//        while (!isspace_ornull(Token[i])) {
//            ++i;
//        }
//        if (i >= Length) {
//            EverythingIsBurning = true;
//            goto ArgCountEnd;
//        }
//        ++ArgCount;
//    }
//    //++ArgCount;
//ArgCountEnd:
//    if (ArgCount < MinArgCount) {
//
//    }
    
    
    char** Args;
    size_t TrueArgCount = 0;
    /*
    * HOLY CRAP THIS IS UTTERLY TERRIBLE
    *
    * I sincerely have no idea WTF this is doing,
    * all I did was just keep hitting it with a
    * proverbial wrench until it vaguely worked
    * without dying instantly.
    *
    * And not only that, but it doesn't allow for stupid
    * amounts of totally legal whitespace in function-like
    * macros, so it'll need to be replaced. For now, I don't
    * care though. :/
    */
    if (MaxArgCount > 0) {
        bool StrtokFirstCall = true;
        size_t ArgCount = 0;
        char* PToken;
        char* OriginalTokenArgs = StringDupe(TokenArgs);
        while (ArgCount < MaxArgCount) {
            PToken = strtok(StrtokFirstCall ? StrtokFirstCall = false, TokenArgs : NULL, " \n");
            if (PToken == NULL) {
                break;
            }
            ++ArgCount;
        }
        if (ArgCount < MinArgCount) {
            PrintErrorLineInfoExit("not enough args for directive", (char*)PreprocessorStrings[DirectiveType], *LineNumber, *Filename);
        }
        StrtokFirstCall = true;
        Args = calloc(ArgCount, sizeof(char*));
        TrueArgCount = ArgCount;
        ArgCount = 0;
        while (ArgCount < TrueArgCount) {
            PToken = strtok(StrtokFirstCall ? StrtokFirstCall = false, OriginalTokenArgs : NULL, ArgDelims);
            if (PToken == NULL) {
                break;
            }
            Args[ArgCount++] = StringDupe(PToken);
        }
        //Args[ArgCount] = NULL;
        ArgCount = 0;
        free(OriginalTokenArgs);
    }
    
    //Of course, the directive type still determines how
    //everything needs to be parsed in the first place...
    switch (DirectiveType) {
        case Line:
            if (TrueArgCount >= 1) {
                *LineNumber = atol(Args[0]);
            }
            if (TrueArgCount >= 2) {
                *Filename = StringDupe(Args[1]);
            }
            break;
        case Define:
            
            break;
        case Include:

            break;
        
            
    }
    /*size_t j = 0;
    while (j < TrueArgCount) {
        if (Args[j] != NULL) {
            free(Args[j]);
        }
        ++j;
    }*/
    size_t j;
    for (j = 0; j < TrueArgCount; ++j) {
        if (Args[j] != NULL) {
            free(Args[j]);
        }
    }

    //char* strtok(Token, " \t\v\f\n");
}

//
//I'm largely basing this on this documentation of C compilers:
//https://en.cppreference.com/w/c/language/translation_phases
//
void tokenize(char* Filename, ParseMode Mode) {

    if (Mode == Compile && Source != NULL) {
        FreeSourceFiles(Source);
        Source = NULL;
    }
   
    signed char temp = 0;
    signed char temp2;

    FILE* ECL_IN_FILE = SquitFOpen(Filename, "rb");
    int_fast8_t TokenizerMode = MainProcessor;
    StringStream ECL_Buffer = (StringStream){.String = NULL, .CurrentPos = NULL, .Pos = 0, .Size = 0};
    
    //
    //START PHASE 1 AND 2
    //
    //TODO: Merge these two loops somehow. It *has* to be possible, right?
    //
    /*while (temp = fgetc(ECL_IN_FILE), temp != EOF) {
        ++ECL_Buffer.Size;
        switch (temp) {
            case Trigraph:
                temp2 = ProcessTrigraph(ECL_IN_FILE);
                if (temp2 == '\?') {
                    continue;
                } else if (temp2 != Escape) {
                    --ECL_Buffer.Size;
                    continue;
                }
            case Escape:
                temp2 = fgetc(ECL_IN_FILE);
                if (temp2 == Newline) {
                    --ECL_Buffer.Size;
                    continue;
                }
                ungetc(temp2, ECL_IN_FILE);
            default:
                continue;
        }
    }*/
#ifndef SEEK_END_IS_STUPID
    fseek(ECL_IN_FILE, 0, SEEK_END);
#else
    //Apparently it's not required to "meaningfully support" SEEK_END
    //according to some documentation? Meh, have an option then.
    while (fgetc(ECL_IN_FILE) != EOF);
#endif
    ECL_Buffer.Size = ftell(ECL_IN_FILE);
    rewind(ECL_IN_FILE);
    //Allocated size is +2 to allow enough space
    //for an extra newline and a null terminator
    ECL_Buffer.String = callocNonNull(ECL_Buffer.Size + 2, sizeof(wchar_t));
    size_t i;
    size_t tempsize;
    //wctrans("");
    for (i = 0; i < ECL_Buffer.Size; ++i) {
        temp = fgetc(ECL_IN_FILE);
        if (temp == WEOF) {
            break;
        }
        switch (temp) {
            case Trigraph:
                temp2 = ProcessTrigraph(ECL_IN_FILE);
                if (temp2 == '\?') {
                    ECL_Buffer.String[i] = '\?';
                } else if (temp2 != Escape) {
                    --i;
                    continue;
                }
            case Escape:
                temp2 = fgetc(ECL_IN_FILE);
                if (temp2 == Newline) {
                    --i;
                    continue;
                }
                ungetc(temp2, ECL_IN_FILE);
            default:
                ECL_Buffer.String[i] = temp;
                continue;
        }
    }
    fclose(ECL_IN_FILE);
    ECL_Buffer.String[i] = '\n'; //This should be safe to do because the buffer was allocated a bit larger
    //
    //END PHASE 1 AND 2
    //

#if 0
    SourceFile* CurrentSourceFile;

    *NextSourceFile = mallocNonNull(sizeof(SourceFile));
    CurrentSourceFile = *NextSourceFile;
    NextSourceFile = &(CurrentSourceFile->Next);
    CurrentSourceFile->TrueFilename = CurrentSourceFile->Filename = StringDupe(Filename);
    CurrentSourceFile->Next = NULL;
    CurrentSourceFile->Tokens = NULL;
    CurrentSourceFile->NextToken = &(CurrentSourceFile->Tokens);

    //
    //START PHASE 3 AND 4
    //
    enum {
        NoComment,
        BlockComment = '*',
        LineComment = '/'
    } CommentMode = NoComment;
    enum {
        NoString,
        String = '\"'
    } StringMode = NoString;
    enum {
        NoCharacter,
        Character = '\''
    } CharacterMode = NoCharacter;
    //SourceToken* CurrentToken = AddSourceToken(CurrentSourceFile);
    bool StartTokenAtNext = true;
    char* TokenStart = NULL;
    char EndTokenCompare;
    char EndTokenCompare2;
    char* PreprocessorTemp;
    SourceToken TempToken = (SourceToken){ .Length = 0, .TokenType = Unknown, .Line = 0, .Next = NULL, .Token = NULL };
    ECL_Buffer.CurrentPos = ECL_Buffer.String;
    while (ECL_Buffer.Pos < ECL_Buffer.Size) {
        //temp = StringStreamNextChar(ECL_Buffer);
        temp = ECL_Buffer.String[ECL_Buffer.Pos++];
        switch (TempToken.TokenType) {
            case Preprocessor:
                do {
                    ++TempToken.Length;
                } while (ECL_Buffer.String[ECL_Buffer.Pos++] != Newline);
                PreprocessorTemp = StringDupeToSize(TokenStart, ++TempToken.Length);
                RunPreprocessor(PreprocessorTemp, &TempToken.Line, &CurrentSourceFile->Filename, CurrentSourceFile->TrueFilename);
                free(PreprocessorTemp);
                ++TempToken.Line;
                break;//BreakToAddToken
            case String: case Character: case BlockComment:
                ++TempToken.Length;
                if (temp == EndTokenCompare && temp2 != EndTokenCompare2) {
                    break;//BreakToAddToken
                }
                temp2 = temp;
                continue;
            case LineComment:
                if (temp != Newline) continue;
                ++TempToken.Line;
                break;//BreakToAddToken
            case Unknown:
                switch (temp) {
                    case Preprocessor:
                        TempToken.TokenType = Preprocessor;
                        TokenStart = &ECL_Buffer.String[ECL_Buffer.Pos];
                        continue;
                    case String:
                        TempToken.TokenType = String;
                        TokenStart = &ECL_Buffer.String[ECL_Buffer.Pos];
                        temp2 = temp;
                        EndTokenCompare2 = Escape;
                        EndTokenCompare = String;
                        continue;
                    case Character:
                        TempToken.TokenType = Character;
                        TokenStart = &ECL_Buffer.String[ECL_Buffer.Pos];
                        temp2 = temp;
                        EndTokenCompare2 = Escape;
                        EndTokenCompare = Character;
                        continue;
                    case Space: case HorizontalTab: case VerticalTab: case FormFeed:
                        //while (isspace_tokenize(StringStreamNextChar(ECL_Buffer)));
                        while (isspace_tokenize(ECL_Buffer.String[ECL_Buffer.Pos++]));
                        break;//BreakToAddToken
                    case Newline:
                        ++TempToken.Line;
                        break;//BreakToAddToken
                    case Comment:
                        temp2 = ECL_Buffer.String[ECL_Buffer.Pos++];
                        switch (temp2) {
                            case BlockComment:
                                TempToken.TokenType = BlockComment;
                                EndTokenCompare2 = '*';
                                EndTokenCompare = '/';
                                temp2 = '\0';
                                continue;
                            case LineComment:
                                TempToken.TokenType = LineComment;
                                continue;
                        }
                    default:
                        ++TempToken.Length;
                        continue;
                }
        }
        if (TempToken.Length == 0) {
            continue;
        }
        if (TempToken.TokenType != Preprocessor) {
            TempToken.Token = StringDupeToSize(TokenStart, TempToken.Length);
            AppendToken(CurrentSourceFile, &TempToken);
        }
        TempToken = (SourceToken){ .Length = 0, .TokenType = Unknown, .Line = TempToken.Line, .Next = NULL, .Token = NULL };
        
        //switch (CommentMode) {
        //    case BlockComment:
        //        if (temp != '*' || ECL_Buffer.String[ECL_Buffer.Pos++] != '/') continue;
        //        CommentMode = NoComment;
        //        continue;
        //    case LineComment:
        //        if (temp != '\n') continue;
        //        CommentMode = NoComment;
        //        continue;
        //    default:
        //        switch (temp) {
        //            case Space: case HorizontalTab: case VerticalTab: case FormFeed:
        //                //EndToken
        //                break;
        //            case Preprocessor:
        //                break;
        //            case Comment:
        //                temp2 = ECL_Buffer.String[ECL_Buffer.Pos++];
        //                switch (temp2) {
        //                    case BlockComment:
        //                        CommentMode = BlockComment;
        //                        continue;
        //                    case LineComment:
        //                        CommentMode = LineComment;
        //                        continue;
        //                }

        //        }
        //}
    }

#else

    FILE* ECL_IN_FILE_TEMP = tmpfile();
    //fwrite(ECL_Buffer.String, sizeof(char), ECL_Buffer.Size, ECL_IN_FILE_TEMP);
    fprintf(ECL_IN_FILE_TEMP, "%s", ECL_Buffer.String);
    rewind(ECL_IN_FILE_TEMP);

    ECLDefine* DefinesLast;
    ECLDefine** DefinesAccess;
    ECLDefine* NextDefine;

    ECLToken* TokensLast;
    ECLToken** TokensAccess;
    ECLToken* NextToken;

    char* InterpretString = NULL;
    char* temp_string;

    static flag ProcessLine = true;
    flag PreviousProcessLine;
    fpos_t PosRestore;
//    ECL_Buffer.Pos = 0;
//    do {
//        temp = ECL_Buffer.String[ECL_Buffer.Pos++];
//        switch (temp) {
//            case Preprocessor:
//                if (PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString) != NULL) {
//                    if (ProcessLine) {
//                        if (strcmp(InterpretString, "define") == 0) {
//                            DefinesAccess = &Defines;
//                            DefinesLast = NULL;
//                            while (*DefinesAccess != NULL) {
//                                DefinesLast = *DefinesAccess;
//                                DefinesAccess = &((*DefinesAccess)->next);
//                            }
//                            NextDefine = NewECLDefine();
//                            *DefinesAccess = NextDefine;
//#pragma warning(suppress: 28182)
//                            NextDefine->previous = DefinesLast;
//                            NextDefine->name = StringDupe(PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString));
//                            temp_string = PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            if (temp_string != NULL) {
//                                NextDefine->value = StringDupe(temp_string);
//                                continue;
//                            } else {
//                                NextDefine->value = StringDupe("1");
//                                continue;
//                            }
//                        } else if (strcmp(InterpretString, "include") == 0) {
//                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            tokenize(strtok(InterpretString, "\""), Tokenize);
//                            continue;
//                        } else if (strcmp(InterpretString, "if") == 0) {
//                            NotImplemented(if);
//                            continue;
//                        } else if (strcmp(InterpretString, "ifdef") == 0) {
//                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            DefinesAccess = &Defines;
//                            ProcessLine = false;
//                            while (*DefinesAccess != NULL && ProcessLine == false) {
//                                ProcessLine = strcmp((*DefinesAccess)->name, InterpretString) == 0;
//                                DefinesAccess = &(*DefinesAccess)->next;
//                            }
//                            continue;
//                        } else if (strcmp(InterpretString, "ifndef") == 0) {
//                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            DefinesAccess = &Defines;
//                            while (*DefinesAccess != NULL) {
//                                if (strcmp((*DefinesAccess)->name, InterpretString) == 0) {
//                                    ProcessLine = false;
//                                    break;
//                                }
//                                DefinesAccess = &(*DefinesAccess)->next;
//                            }
//                            continue;
//                        } else if (strcmp(InterpretString, "undef") == 0) {
//                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            DefinesAccess = &Defines;
//                            DefinesLast = NULL;
//                            while (*DefinesAccess != NULL) {
//                                DefinesLast = *DefinesAccess;
//                                if (strcmp((*DefinesAccess)->name, InterpretString) == 0) {
//                                    (*DefinesAccess)->next->previous = DefinesLast;
//                                    NextDefine = DefinesLast->next;
//                                    DefinesLast->next = (*DefinesAccess)->next;
//                                    free(NextDefine);
//                                    break;
//                                }
//                                DefinesAccess = &(*DefinesAccess)->next;
//                            }
//                            continue;
//                        } else if (strcmp(InterpretString, "line") == 0) {
//                            NotImplemented(line);
//                            continue;
//                        } else if (strcmp(InterpretString, "error") == 0) {
//                            NotImplemented(error);
//                            continue;
//                        } else if (strcmp(InterpretString, "pragma") == 0) {
//                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
//                            if (strcmp(InterpretString, "once") == 0) {
//                                PreviousProcessLine = ProcessLine;
//                                DefinesAccess = &Defines;
//                                while (*DefinesAccess != NULL) {
//                                    if (strcmp((*DefinesAccess)->name, Filename) == 0) {
//                                        ProcessLine = false;
//                                        break;
//                                    }
//                                    DefinesAccess = &(*DefinesAccess)->next;
//                                }
//                                if (ProcessLine == false) {
//                                    ProcessLine = PreviousProcessLine;
//                                    fclose(ECL_IN_FILE_TEMP);
//                                    return;
//                                }
//                                DefinesAccess = &Defines;
//                                DefinesLast = NULL;
//                                while (*DefinesAccess != NULL) {
//                                    DefinesLast = *DefinesAccess;
//                                    DefinesAccess = &((*DefinesAccess)->next);
//                                }
//                                NextDefine = NewECLDefine();
//                                *DefinesAccess = NextDefine;
//#pragma warning(suppress: 28182)
//                                NextDefine->previous = DefinesLast;
//                                NextDefine->name = StringDupe(Filename);
//                                NextDefine->value = StringDupe("1");
//                                continue;
//                            } else {
//                                printf("Unknown preprocessor directive #pragma %s", InterpretString);
//                                continue;
//                            }
//                        } else if (strcmp(InterpretString, "else") == 0) {
//                            ProcessLine = false;
//                            continue;
//                        } else if (strcmp(InterpretString, "elif") != 0 &&
//                                   strcmp(InterpretString, "endif") != 0) {
//                            printf("Unknown preprocessor directive #%s", InterpretString);
//                            continue;
//                        }
//                    } else {
//                        if (strcmp(InterpretString, "else") == 0) {
//                            ProcessLine = true;
//                            continue;
//                        } else if (strcmp(InterpretString, "elif") == 0) {
//                            NotImplemented(elif);
//                            continue;
//                        } else if (strcmp(InterpretString, "endif") == 0) {
//                            ProcessLine = true;
//                            continue;
//                        } else if (strcmp(InterpretString, "define") != 0 &&
//                                   strcmp(InterpretString, "include") != 0 &&
//                                   strcmp(InterpretString, "if") != 0 &&
//                                   strcmp(InterpretString, "ifdef") != 0 &&
//                                   strcmp(InterpretString, "ifndef") != 0 &&
//                                   strcmp(InterpretString, "undef") != 0 &&
//                                   strcmp(InterpretString, "line") != 0 &&
//                                   strcmp(InterpretString, "error") != 0 &&
//                                   strcmp(InterpretString, "pragma") != 0
//                                   ) {
//                            printf("Unknown preprocessor directive #%s", InterpretString);
//                            continue;
//                        }
//                    }
//                }
//                break;
//            case String:
//                break;
//            case Character:
//                break;
//            case Comment:
//                break;
//            default:
//                break;
//        }
//    } while (1);
//    
//    do {
//        temp = fgetc(ECL_IN_FILE_TEMP);
//        switch (temp) {
//            case EOF:
//                goto BreakTokenizerLoop;
//            case '?':
//                TokenizerMode = Trigraph;
//                break;
//            case '\n': case '\r':
//                TokenizerMode = Newline;
//                break;
//            case '#':
//                TokenizerMode = Preprocessor;
//                break;
//            case '\"':
//                TokenizerMode = String;
//                break;
//            case '\'':
//                TokenizerMode = Character;
//                break;
//            case '\\':
//                TokenizerMode = Escape;
//                break;
//            default:
//                TokenizerMode = MainProcessor;
//        }
//        switch (TokenizerMode) {
//            if (0) {
//            case Trigraph:
//                if (ProcessTrigraph(ECL_IN_FILE_TEMP)) {
//                    continue;
//                }
//            } else if (0) {
//            case Escape:
//                //char TempBuffer[3] = { '\0', '\0', '\0' };
//                fgetpos(ECL_IN_FILE_TEMP, &PosRestore);
//                //char temp2 = fgetc(ECL_IN_FILE_TEMP);
//            }
//            
//            case MainProcessor:
//                break;
//            case Preprocessor:
//                break;
//            default:
//                break;
//        }
//    } while (1);
//BreakTokenizerLoop:

    size_t CharCount = 0;
    do {
        temp = fgetc(ECL_IN_FILE_TEMP);
        CharCount = ftell(ECL_IN_FILE_TEMP);
        if (feof(ECL_IN_FILE_TEMP) || ferror(ECL_IN_FILE_TEMP)) {
            break;
        }
        /*fgetpos(ECL_IN_FILE_TEMP, &PosRestore);
        if (temp == '?') {
            
        } else if (temp == '\r') {
            temp2 = fgetc(ECL_IN_FILE_TEMP);
            if (feof(ECL_IN_FILE_TEMP) || ferror(ECL_IN_FILE_TEMP)) {
                fsetpos(ECL_IN_FILE_TEMP, &PosRestore);
            } else {
                if (temp2 != '\n') {
                    ungetc(temp2, ECL_IN_FILE_TEMP);
                }
                ungetc('\n', ECL_IN_FILE_TEMP);
            }
        } else if (temp == '\\') {

        }*/
        switch (temp) {
            case BlankLine:
                continue;
            case PreprocessorStarter:
                if (PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString) != NULL) {
                    if (ProcessLine) {
                        if (strcmp(InterpretString, "define") == 0) {
                            DefinesAccess = &Defines;
                            DefinesLast = NULL;
                            while (*DefinesAccess != NULL) {
                                DefinesLast = *DefinesAccess;
                                DefinesAccess = &((*DefinesAccess)->next);
                            }
                            NextDefine = NewECLDefine();
                            *DefinesAccess = NextDefine;
#pragma warning(suppress: 28182)
                            NextDefine->previous = DefinesLast;
                            NextDefine->name = StringDupe(PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString));
                            temp_string = PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            if (temp_string != NULL) {
                                NextDefine->value = StringDupe(temp_string);
                                continue;
                            } else {
                                NextDefine->value = StringDupe("1");
                                continue;
                            }
                        } else if (strcmp(InterpretString, "include") == 0) {
                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            tokenize(strtok(InterpretString, "\""), Tokenize);
                            continue;
                        } else if (strcmp(InterpretString, "if") == 0) {
                            NotImplemented(if);
                            continue;
                        } else if (strcmp(InterpretString, "ifdef") == 0) {
                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            DefinesAccess = &Defines;
                            ProcessLine = false;
                            while (*DefinesAccess != NULL && ProcessLine == false) {
                                ProcessLine = strcmp((*DefinesAccess)->name, InterpretString) == 0;
                                DefinesAccess = &(*DefinesAccess)->next;
                            }
                            continue;
                        } else if (strcmp(InterpretString, "ifndef") == 0) {
                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            DefinesAccess = &Defines;
                            while (*DefinesAccess != NULL) {
                                if (strcmp((*DefinesAccess)->name, InterpretString) == 0) {
                                    ProcessLine = false;
                                    break;
                                }
                                DefinesAccess = &(*DefinesAccess)->next;
                            }
                            continue;
                        } else if (strcmp(InterpretString, "undef") == 0) {
                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            DefinesAccess = &Defines;
                            DefinesLast = NULL;
                            while (*DefinesAccess != NULL) {
                                DefinesLast = *DefinesAccess;
                                if (strcmp((*DefinesAccess)->name, InterpretString) == 0) {
                                    (*DefinesAccess)->next->previous = DefinesLast;
                                    NextDefine = DefinesLast->next;
                                    DefinesLast->next = (*DefinesAccess)->next;
                                    free(NextDefine);
                                    break;
                                }
                                DefinesAccess = &(*DefinesAccess)->next;
                            }
                            continue;
                        } else if (strcmp(InterpretString, "line") == 0) {
                            NotImplemented(line);
                            continue;
                        } else if (strcmp(InterpretString, "error") == 0) {
                            NotImplemented(error);
                            continue;
                        } else if (strcmp(InterpretString, "pragma") == 0) {
                            PreprocessorReadText(ECL_IN_FILE_TEMP, &InterpretString);
                            if (strcmp(InterpretString, "once") == 0) {
                                PreviousProcessLine = ProcessLine;
                                DefinesAccess = &Defines;
                                while (*DefinesAccess != NULL) {
                                    if (strcmp((*DefinesAccess)->name, Filename) == 0) {
                                        ProcessLine = false;
                                        break;
                                    }
                                    DefinesAccess = &(*DefinesAccess)->next;
                                }
                                if (ProcessLine == false) {
                                    ProcessLine = PreviousProcessLine;
                                    fclose(ECL_IN_FILE_TEMP);
                                    return;
                                }
                                DefinesAccess = &Defines;
                                DefinesLast = NULL;
                                while (*DefinesAccess != NULL) {
                                    DefinesLast = *DefinesAccess;
                                    DefinesAccess = &((*DefinesAccess)->next);
                                }
                                NextDefine = NewECLDefine();
                                *DefinesAccess = NextDefine;
#pragma warning(suppress: 28182)
                                NextDefine->previous = DefinesLast;
                                NextDefine->name = StringDupe(Filename);
                                NextDefine->value = StringDupe("1");
                                continue;
                            } else {
                                printf("Unknown preprocessor directive #pragma %s", InterpretString);
                                continue;
                            }
                        } else if (strcmp(InterpretString, "else") == 0) {
                            ProcessLine = false;
                            continue;
                        } else if (strcmp(InterpretString, "elif") != 0 &&
                                   strcmp(InterpretString, "endif") != 0) {
                            printf("Unknown preprocessor directive #%s", InterpretString);
                            continue;
                        }
                    } else {
                        if (strcmp(InterpretString, "else") == 0) {
                            ProcessLine = true;
                            continue;
                        } else if (strcmp(InterpretString, "elif") == 0) {
                            NotImplemented(elif);
                            continue;
                        } else if (strcmp(InterpretString, "endif") == 0) {
                            ProcessLine = true;
                            continue;
                        } else if (strcmp(InterpretString, "define") != 0 &&
                                   strcmp(InterpretString, "include") != 0 &&
                                   strcmp(InterpretString, "if") != 0 &&
                                   strcmp(InterpretString, "ifdef") != 0 &&
                                   strcmp(InterpretString, "ifndef") != 0 &&
                                   strcmp(InterpretString, "undef") != 0 &&
                                   strcmp(InterpretString, "line") != 0 &&
                                   strcmp(InterpretString, "error") != 0 &&
                                   strcmp(InterpretString, "pragma") != 0
                                   ) {
                            printf("Unknown preprocessor directive #%s", InterpretString);
                            continue;
                        } else {
                            
                        }
                    }
                }
                continue;
            case CommentToken:
            default:
                if (MainProcessorReadText(ECL_IN_FILE_TEMP, &InterpretString) != NULL && ProcessLine) {
                    MainProcessorTokenize(&InterpretString, &Defines, &Tokens);
                    TokensAccess = &Tokens;
                    TokensLast = NULL;
                    while (*TokensAccess != NULL) {
                        TokensLast = *TokensAccess;
                        TokensAccess = &((*TokensAccess)->next);
                    }
                    NextToken = NewECLToken();
                    *TokensAccess = NextToken;
#pragma warning(suppress: 28182)
                    NextToken->previous = TokensLast;
                    NextToken->value = StringDupe(";");
                    continue;
                }
                continue;
        }
    } while (1);
    free(InterpretString);
    fclose(ECL_IN_FILE_TEMP);
    if (Mode == Tokenize) {
        return;
    }
    FreeDefines(Defines);
    temp_string = strtok(Filename, ".");
    //compile(temp_string, Tokens);
    //FreeTokens(Tokens);
    return;
#endif
}