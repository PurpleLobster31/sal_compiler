#include "token.h"

#include <string.h>

Token token_make(TokenType type, const char *lexeme, int line) {
    Token token;
    size_t i = 0;

    token.type = type;
    token.line = line;

    if (lexeme != NULL) {
        while (lexeme[i] != '\0' && i < TOKEN_LEXEME_MAX - 1) {
            token.lexeme[i] = lexeme[i];
            i++;
        }
    }
    
    token.lexeme[i] = '\0';
    return token;
}

const char *token_type_name(TokenType type) {
    switch (type) {
        case sEOF:         return "sEOF";
        case sERRO:        return "sERRO";

        case sIDENTIF:     return "sIDENTIF";
        case sCTEINT:      return "sCTEINT";
        case sCTECHAR:     return "sCTECHAR";
        case sSTRING:      return "sSTRING";

        case sMODULE:      return "sMODULE";
        case sGLOBALS:     return "sGLOBALS";
        case sLOCALS:      return "sLOCALS";
        case sPROC:        return "sPROC";
        case sFN:          return "sFN";
        case sMAIN:        return "sMAIN";
        case sSTART:       return "sSTART";
        case sEND:         return "sEND";
        case sINT:         return "sINT";
        case sBOOL:        return "sBOOL";
        case sCHAR:        return "sCHAR";
        case sIF:          return "sIF";
        case sELSE:        return "sELSE";
        case sMATCH:       return "sMATCH";
        case sWHEN:        return "sWHEN";
        case sOTHERWISE:   return "sOTHERWISE";
        case sFOR:         return "sFOR";
        case sTO:          return "sTO";
        case sSTEP:        return "sSTEP";
        case sDO:          return "sDO";
        case sLOOP:        return "sLOOP";
        case sWHILE:       return "sWHILE";
        case sUNTIL:       return "sUNTIL";
        case sPRINT:       return "sPRINT";
        case sSCAN:        return "sSCAN";
        case sRETURN:      return "sRETURN";
        case sTRUE:        return "sTRUE";
        case sFALSE:       return "sFALSE";

        case sATRIB:       return "sATRIB";
        case sSOMA:        return "sSOMA";
        case sSUBRAT:      return "sSUBRAT";
        case sMULT:        return "sMULT";
        case sDIV:         return "sDIV";
        case sIGUAL:       return "sIGUAL";
        case sDIFERENTE:   return "sDIFERENTE";
        case sMAIOR:       return "sMAIOR";
        case sMENOR:       return "sMENOR";
        case sMAIORIG:     return "sMAIORIG";
        case sMENORIG:     return "sMENORIG";
        case sAND:         return "sAND";
        case sOR:          return "sOR";
        case sNEG:         return "sNEG";
        case sIMPLIC:      return "sIMPLIC";
        case sPTOPTO:      return "sPTOPTO";

        case sABREPAR:     return "sABREPAR";
        case sFECHAPAR:    return "sFECHAPAR";
        case sABRECOL:     return "sABRECOL";
        case sFECHACOL:    return "sFECHACOL";
        case sVIRG:        return "sVIRG";
        case sDPTO:        return "sDPTO";
        case sPVIRG:       return "sPVIRG";
    }

    return "sERRO";
}