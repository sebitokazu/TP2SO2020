#include "calculator.h"

static double evaluatePostFija(char *exp, int* error_ptr);
static double doTheOperation(double d1, double d2, char op, int* error_ptr);
static void infijaToPostFija(char *src, char *dst, int* error_ptr);
static int isNum(char c);
static int isOperator(char c);
static int isAParenthesis(char c);
static int precedence(char top, char current);
static int asignatePrecedenceValue(char operator);
static double evaluate(char *exp, int* error_ptr);
static int validateChar(char c);


#define BUFFER_SIZE 50

static char exp[BUFFER_SIZE] = {0};
static int i=0, ctrl=0;

static char precedenceMat[6][7] = {{1,1,0,0,0,0,1}, {1,1,0,0,0,0,1}, {1,1,1,1,0,0,1}, {1,1,1,1,0,0,1}, {1,1,1,1,0,0,1}, {0,0,0,0,0,0,0}};

void initCalculator(){
    char c=0;
    while(c != '=' && i < BUFFER_SIZE-1){
        c = getChar();
        switch(c){
            case 0: break;
            case -1: ctrl=0; break;
            case 1: ctrl=1; break;
            case ' ': //no permite mas de un espacio
                if(i>0 && exp[i-1] != ' '){
                    exp[i++] = ' ';
                    putChar(c);
                }
                break;
            case '\b': 
                if(i!=0){
                    backspace();
                    exp[--i]=0;
                }
                break;
            case 'v':   //limpia la pantalla
                clearScreen(); 
                exp[0] = 0;
                i=0;
                break;
            case 'l':   //borra la expresion actual, que todavia no fue calculada
                while(i != 0){
                    backspace();
                    exp[--i] = 0;
                }
                break;
            default:
                if(ctrl && c=='s'){
                    changeScreen();
                    //return;
                }
                if(validateChar(c)){
                    putCharC(c,229,77,43);
                    exp[i++] = c;
                }
        }
    }

    if(i==BUFFER_SIZE-1)
        printf(" We are not CASIO, full buffer");
    else{
        exp[--i] = 0;   //sin el =
        i=0;        //por si se produce excepcion de div por 0 al evaluar
        int error = 0;
        double d = evaluate(exp, &error);
        if(error)
            printf(" La expresion ingresada es incorrecta");
        else{
            char resOut[100];
            floatToString((float)d, resOut, 4);
            putChar(' ');
            printf(resOut);
        }
    }
    i=0;
    enter();
    initCalculator();//Podriamos hacer un while(1) que abarque todo el codigo 
                    //de initCalculator() para evitar la recursion infinita de funciones
}

static int validateChar(char c){
    if(isNum(c) || isOperator(c) || c == '.' || c == '=')
        return 1;
    return 0;
}

/*evaluate: Evalua la expresion ingresada por el usuario. Si hay algun error en ella
setea error_ptr en 1. Para evaluar, transforma a notacion postfija la expresion
y luego la evalua.
*/
static double evaluate(char* exp, int* error_ptr){
    int size = strlen(exp);
    char postFija[size];
    infijaToPostFija(exp,postFija, error_ptr);
    if(*error_ptr)
        return -1;
    return evaluatePostFija(postFija, error_ptr);
}

/*evaluatePostfija: Realiza la evaluacion de la expresion ingresada por el usuario,
la cual previamente fue transformada a notacion postfija. Si hay algun error, setea
la error_ptr en 1.
*/
static double evaluatePostFija(char *exp, int* error_ptr){
    int i=0;
    int STACK_SIZE = 20;
    double doubleStack[STACK_SIZE];
    int stackSize = 0;
    double tempD;
    while(exp[i] != 0 && stackSize < STACK_SIZE){
        if(isNum(exp[i]) || (exp[i] == '-' && isNum(exp[i+1])) ){
            tempD = stringToDouble(exp+i);
            doubleStack[stackSize++] = tempD;
            while(exp[i] != ' ') i++;
            i++;
        }
        else if(isOperator(exp[i])){
            double d1, d2;
            if(stackSize == 0){
                *error_ptr = 1;
                return -1;
            }
            d2 = doubleStack[--stackSize];
            if(stackSize == 0){
                *error_ptr = 1;
                return -1;
            }
            d1 = doubleStack[--stackSize];
            tempD = doTheOperation(d1, d2, exp[i], error_ptr);
            doubleStack[stackSize++] = tempD;
            while(exp[i] != ' ' && exp[i] != 0)
                i++;
        }
        else i++;
    }
    if(stackSize != 1 || stackSize == STACK_SIZE){
        *error_ptr=1;
        return -1;
    }
    return doubleStack[0];
}

static double doTheOperation(double d1, double d2, char op, int* error_ptr){
    double temp = 1;
    switch(op){
        case '+': 
            return d1 + d2;
        case '-': 
            return d1 - d2;
        case '*': 
            return d1 * d2;
        case '/':
            if(d2==0){
                *error_ptr=1;
                return -1;
            }
            return d1 / d2;
        case '^':
            while(d2-->0)
                temp*= d1;
            return temp;
        default:
            *error_ptr=1;
            return -1;
    }
    return -1;
}

/*infijaToPostfija: Transforma la expresion ingresada por el usuario a notacio
postfija. Si la misma es incorrecta setea error_ptr en 1.
*/
static void infijaToPostFija(char *src, char *dst, int* error_ptr){
    char operatorStack[100];
    int stackSize = 0;
    int error = 0, point_qty=0;
    int i=0, j=0;
    char previousToken=-1, currentToken=-1; //0 if number, 1 if '(', 2 if ')', else 3
    while(src[i] != 0 && !error){
        if(src[i] == '.') error = 1;
        else{
        if(isNum(src[i]) || (src[i] == '-' && isNum(src[i+1])) ){ //accept negatives
            currentToken = 0;
            if(previousToken == 0)
                error=1;
            if(src[i] == '-')
                dst[j++] = src[i++];
            while( isNum(src[i]) || src[i]=='.'){
                if(src[i] == '.')
                    point_qty++;
                dst[j++] = src[i++];
            }
            if(point_qty > 1 || src[i] != ' ')
                error=1;
            
            point_qty=0;
            dst[j++] = ' ';
        }
        else if(isOperator(src[i])){
            currentToken = isAParenthesis(src[i]);
            if ( previousToken==0 && currentToken==1 ){
                error = 1;
            }
            while(stackSize != 0 && precedence(operatorStack[stackSize-1], src[i])){
                dst[j++] = operatorStack[stackSize-1];
                stackSize--;
                dst[j++] = ' ';
            }
            
            if(currentToken == 2){
                if(stackSize == 0){ // no opening parenthesis
                    error = 1;
                }
            stackSize--;
            }else{
                operatorStack[stackSize++] = src[i];
            }
        }
        i++;
        previousToken = currentToken;
        }
    }
    dst[j] = 0;
    *error_ptr = error;
}





static int isNum(char c){
    if(c<48) return false;
    if(c>57) return false;
    return true;
}

static int isOperator(char c){
    if(c == '+' || c == '-' || c == '/' || c == '*' || c == '(' || c == ')' || c == '^' )
        return true;
    return false;
}

/*isAParenthesis: Retorna 1 si el parametro es un parentesis que abre, 
2 si es uno que cierra o 3 si es otra cosa.
*/
static int isAParenthesis(char c){
    if(c == '(') return 1;
    else if (c == ')') return 2;
    return 3;
}

/*precedence: Retorna cual de los dos operadores ingresados como
parametro tiene mayor precedencia. Para ello utiliza una matriz de precedencia.
*/
static int precedence(char top, char current){
    if(!isOperator(top) || !isOperator(current)) return -1;
    int row = asignatePrecedenceValue(top);
    int col = asignatePrecedenceValue(current);
    return precedenceMat[row][col];
}

static int asignatePrecedenceValue(char operator){
    switch(operator){
        case '+': return 0;
        case '-': return 1;
        case '*': return 2;
        case '/': return 3;
        case '^': return 4;
        case '(': return 5;
        case ')': return 6;
    }
    return -1;
}




