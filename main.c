#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct list {
    char elem;
    struct list* next;
} list;

typedef struct list_of_indef_to_mul {
    char* indef;
    struct list_of_indef_to_mul* next;
} list_of_indef_to_mul;

typedef struct number {
    double value_mul;
    double term;
    list_of_indef_to_mul* mul;
} number;

typedef struct symb {
    char imp_char;
    number* operand;
} symb;

typedef struct list_of_symb {
    symb* elem;
    struct list_of_symb* next;
} list_of_symb;

typedef struct list_of_operands {
    number* elem;
    struct list_of_operands* next;
} list_of_operands;

typedef struct values_of_var_list {
    char* indef;
    double value;
    struct values_of_var_list* next;
} values_of_var_list;

values_of_var_list* values= NULL;

void initNumb(number** N) {
    (*N)->term = 0;
    (*N)->mul = NULL;
}

void addElemToVarList( values_of_var_list** L, char* indf, double val ) {
    if ( *L == NULL ){
        *L= (values_of_var_list*) malloc(sizeof(values_of_var_list));
        
        (*L)->indef= (char*) malloc(strlen(indf) * sizeof(char) ) ;
        strcpy((*L)->indef, indf);
        
        (*L)->value= val;
        (*L)->next= NULL;
    }
    else {
        addElemToVarList(&((*L)->next), indf, val);
    }
}

void deleteVarList( values_of_var_list** L ){
    if ( *L == NULL )
        return;
    
    free((*L)->indef);
    (*L)->indef= NULL;
    values_of_var_list* L_temp= (*L)->next;
    free(*L);
    *L= NULL;
    deleteVarList( &L_temp );
}

int searchValueOfVar(values_of_var_list* L, char* indf) {
    long x = 0;
    int N = 0;
    while (L != NULL) {
        N++;
        if ((x = strlen(L->indef))  == strlen(indf)) {
            if (!memcmp(L->indef, indf, x)){
                return N;
            }
        }
        L = L->next;
    }
    return 0;
}

double keyOfValue( values_of_var_list* L, int Key){
    int N = 0;
    while ( L != NULL ){
        N++;
        if ( Key == N )
            return L->value;
        L = L->next;
    }
    return 0;
}

void deleteMulList(list_of_indef_to_mul** A) {
    if ((*A) == NULL) {
        return;
    }

    list_of_indef_to_mul* B = (*A)->next;
    free((*A)->indef);
    (*A)->indef= NULL;
    free(*A);
    *A= NULL;
    deleteMulList(&B);

}

void addElemToListMul(list_of_indef_to_mul** C, char* indf) {
    if ((*C) == NULL) {

        (*C) = (list_of_indef_to_mul*)malloc(sizeof(list_of_indef_to_mul));
        (*C)->next = NULL;

        (*C)->indef = (char*)malloc(sizeof(char));
        ((*C)->indef)[0] = '\0';

        strcpy((*C)->indef, indf);

        return;

    }
    else {

        addElemToListMul(&((*C)->next), indf);

    }
}

void deleteNumb(number** A) {
    if ((*A)->mul != NULL) {
        deleteMulList(&((*A)->mul));
        (*A)->mul = NULL;
    }
}

void addNumbers(number** A, number** B, number** C) {
    (*C)->term = ((*A)->term) + ((*B)->term);
}

void mulNumb(number** A, number** B, number** C) {
    (*C)->term = ((*A)->term) * ((*B)->term);
}

void makeReciprocalNumb(number** A) {
    if ((*A)->mul == NULL) {

    }
    (*A)->term = 1 / ((*A)->term);
}

void divNumb(number** A, number** B, number** C) {
    makeReciprocalNumb(B);
    mulNumb(A, B, C);
}

void makeNegNumber(number** A) {
    (*A)->term = (-1) * ((*A)->term);
}

void difNumb(number** A, number** B, number** C) {
    makeNegNumber(B);
    addNumbers(A, B, C);
}

void addSymb(list_of_symb** L, symb* S) {
    if (*L == NULL) {
        (*L) = (list_of_symb*)malloc(sizeof(list_of_symb));
        if (S->imp_char != 0) {
            (*L)->elem = (symb*)malloc(sizeof(symb));

            (*L)->elem->imp_char = S->imp_char;
            (*L)->elem->operand = NULL;

        }
        else {
            (*L)->elem = (symb*)malloc(sizeof(symb));
            (*L)->elem->operand = (number*)malloc(sizeof(number));
            initNumb(&((*L)->elem->operand));
            if ( S->operand->mul == NULL )
                (*L)->elem->operand->term = S->operand->term;
            else
                addElemToListMul(&((*L)->elem->operand->mul), S->operand->mul->indef);
            
        }
        (*L)->next = NULL;

    }
    else {
        addSymb(&((*L)->next), S);
    }
}

int containsDot(char* str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '.') {
            return 1;
        }
    }
    return 0;

}

double strToDouble(char* str, int N) {
    char* mapping_str = (char*)malloc((N + 1) * sizeof(char)); // N+1 для нуля

    for (int i = 0; i < N; i++) {
        mapping_str[i] = str[i];
    }
    
    mapping_str[N] = 0;
    
    if (containsDot(mapping_str)){
        double x = atof(mapping_str);
        free(mapping_str);
        return x;
    }
    else {
        double x = atoi(mapping_str);
        free(mapping_str);
        return x;
    }
}

void strToList(char* str, list_of_symb** list_symb) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\n')
            return;
        if (str[i] == '(' || str[i] == ')' || str[i] == '+' || str[i] == '-' || str[i] == '/' || str[i] == '*') {

            symb* S = (symb*)malloc(sizeof(symb));

            S->imp_char = str[i];

            addSymb(list_symb, S);
            free(S);
        }
        else if ( isdigit(str[i]) ) {
            int j = i;

            while (!(str[i + 1] == '(' || str[i + 1] == ')' ||
                     str[i + 1] == '+' || str[i + 1] == '-' ||
                     str[i + 1] == '/' || str[i + 1] == '*' ||
                     str[i + 1] == '\0')) {
                i++;
            }

            symb* S = (symb*)malloc(sizeof(symb));
            S->imp_char = 0;
            S->operand = (number*)malloc(sizeof(number));
            initNumb(&(S->operand));
            S->operand->term = strToDouble(&str[j], i - j + 1);
            addSymb(list_symb, S);
            free(S);

        }
        else if (((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= 'A') && (str[i] <= 'Z'))) {

            int j = i;

            while (!(str[i + 1] == '(' || str[i + 1] == ')' || str[i + 1] == '+'
                  || str[i + 1] == '-' || str[i + 1] == '/' || str[i + 1] == '*'
                  || str[i+1] == 0)) {
                i++;
            }

            int N = i - j + 1;

            char* indef_str = (char*)malloc((N + 1) * sizeof(char));

            for (int k = 0; k < N; k++) {
                indef_str[k] = str[k + j];
            }
            indef_str[N] = 0;

            symb* S = (symb*)malloc(sizeof(symb));
            S->imp_char = 0;
            S->operand = (number*)malloc(sizeof(number));
            initNumb(&(S->operand));
            addElemToListMul(&(S->operand->mul), indef_str);
            addSymb(list_symb, S);

            free(S->operand);
            free(S);
            free(indef_str);
        }
    }
}

void push(list** stack, char c) {
    if ((*stack) == NULL) {
        (*stack) = (list*) malloc(sizeof(list));
        (*stack)->elem = c;
        (*stack)->next = NULL;
        return;
    }
    push(&stack[0]->next, c);
}

char pop(list** stack) {
    if ((*stack) == NULL) {
        return 0;
    }
    if ((*stack)->next == NULL) {
        char c = (*stack)->elem;
        free((*stack));
        (*stack) = NULL;
        return c;
    }
    return pop(&((*stack)->next));
}

char popFromQue(list** queue){
    if ( (*queue) == NULL ){
        return 0;
    }
    char c= (*queue)->elem;
    free(*queue);
    *queue= (*queue)->next;
    return c;
}

void pushNumber(list_of_operands** L, number* N) {
    if ((*L) == NULL) {
        (*L) = (list_of_operands*)malloc(sizeof(list_of_operands));
        (*L)->elem = (number*)malloc(sizeof(number));
      
        (*L)->next = NULL;
        (*L)->elem->term = N->term;
        (*L)->elem->mul= NULL;
        if ( N -> mul != NULL )
            addElemToListMul(&((*L)->elem->mul), N->mul->indef);
    }
    else {
        pushNumber(&((*L)->next), N);
    }
}

void popNumber(list_of_operands** L, number** N) {
    if ((*L)->next == NULL) {
        (*N)->term = (*L)->elem->term;
        if ( (*L)->elem->mul != NULL )
            addElemToListMul(&((*N)->mul), ((*L)->elem->mul->indef) );
       
        free((*L)->elem);
        free((*L));
        *L = NULL;
    }
    else {
        popNumber(&((*L)->next), N);
    }
}

void popFromQueueNumber( list_of_operands** L, number** N) {
        if ((*L) == NULL)
            return;

        (*N)->term = (*L)->elem->term;
        if ( (*L)->elem->mul != NULL )
            addElemToListMul(&((*N)->mul), ((*L)->elem->mul->indef) );

        free((*L)->elem);
        free((*L));
        *L = (*L)->next;
}

void pushToQueueNumber(list_of_operands** L, number* N) {
    if ((*L) == NULL) {
        (*L) = (list_of_operands*)malloc(sizeof(list_of_operands));
        (*L) -> elem = (number*)malloc(sizeof(number));
        (*L) -> next = NULL;
        (*L) -> elem -> term = N -> term;
        (*L) -> elem -> mul = NULL;
        if ( N -> mul != NULL )
            addElemToListMul(&((*L)->elem->mul), N->mul->indef);
    }
    else{
        list_of_operands* T= *L;
        (*L) = (list_of_operands*)malloc(sizeof(list_of_operands));
        (*L)->elem = (number*)malloc(sizeof(number));
        (*L)->next = T;
        (*L)->elem->term = N->term;
        (*L) -> elem -> mul = NULL;
        if ( N->mul != NULL)
            addElemToListMul(&((*L)->elem->mul), N->mul->indef);
    }
}

int multiplyArray(int* a, int n) {
    int mul = 1;
    for (int i = 1; i < n; i++) {
        mul *= a[i];
    }
    return mul;
}

int containsBrackets(char* str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '(') {
            return 1;
        }
    }
    return 0;
}

int checkBrackets(char* str) {
    list* stack = NULL;

    for (int i = 0; str[i]; i++) {
        if (str[i] == '(') {
            push(&stack, '(');
        }

        if (str[i] == ')') {
            if (stack == NULL) {
                return 0;
            }
            else {
                pop(&stack);
            }
        }
    }

    if (stack == NULL) {
        return 1;
    }
    return 0;
}

int checkMissingOperand(char* str) {
    if (str[0] == '*' || str[0] == '/') {
        return 0;
    }

    for (int i = 0; str[i] != 0; i++) {

        if ((str[i] == '(') && (str[i + 1] == '*' || str[i + 1] == '/')) {
            return 0;
        }

        if ( str[i]== '(' && str[i+1] == ')' )
            return 0;

        if ((str[i + 1] == ')') && (str[i] == '*' || str[i] == '+' || str[i] == '-' || str[i] == '/')) {
            return 0;
        }

        if ((str[i] == '*' || str[i] == '+' || str[i] == '-' || str[i] == '/') && (str[i + 1] == '*' || str[i + 1] == '+' || str[i + 1] == '-' || str[i + 1] == '/' || str[i + 1] == 0)) {
            return 0;
        }

    }

    return 1;
}

int checkInvalidOperation(char* str) {
    for (int i = 0; str[i] != 0; i++) {

        if (!(str[i] == '.' || str[i] == '+' || str[i] == '-' || str[i] == '/' || str[i] == '*' || ((str[i] >= '0' && str[i] <= '9') || (((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= 'A') && (str[i] <= 'Z')))) || (str[i] == ')' || str[i] == '('))) {
            printf("1");
            return 0;
        }

    }

    return 1;
}

int checkPassOfOperation(char* str) {
    for (int i = 0; str[i] != 0; i++) {

        if (((str[i] >= '0') && ((str[i] <= '9'))) && str[i + 1] == '(') {
            return 0;
        }

        if (((str[i] >= '0') && (str[i] <= '9')) && (((str[i + 1] >= 'a') && (str[i + 1] <= 'z')) || ((str[i + 1] >= 'A') && (str[i + 1] <= 'Z')))) {
            return 0;
        }

        if ((((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= 'A') && (str[i] <= 'Z'))) && str[i + 1] == '(') {
            return 0;
        }

        if ((str[i] == ')') && (((str[i + 1] >= 'a') && (str[i + 1] <= 'z')) || ((str[i + 1] >= 'A') && (str[i + 1] <= 'Z')))) {
            return 0;
        }

        if ((str[i] == ')') && ((str[i + 1] >= '0') && ((str[i + 1] <= '9')))) {
            return 0;
        }
    }

    return 1;
}

int checkRigthOperands(char* str) {
    int there_is_indf = 0;
    int N = 0;
    int there_is_double = 0;

    for (int i = 0; str[i] != 0; i++) {
        if ((((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= 'A') && (str[i] <= 'Z')))) {
            if (!there_is_indf) {
                there_is_indf++;
                N++;
            }
            else {
                if (N == 6) {
                    printf("0");
                    return 0;
                }
                else {
                    N++;
                }
            }
        }
        if ((str[i] >= '0') && (str[i] <= '9')) {
            if (there_is_indf) {
                if (N == 6) {
                    printf("5");
                    return 0;
                }
                else {
                    N++;
                }
            }
        }
        if (str[i] == ')' || str[i] == '(' || str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') {
            if (there_is_indf) {
                there_is_indf = 0;
                N = 0;
            }
        }

        if ((str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == ')' || str[i] == '(') && str[i + 1] == '.') {
            printf("1");
            return 0;
        }

        if ((str[i + 1] == '+' || str[i + 1] == '-' || str[i + 1] == '*' || str[i + 1] == '/' || str[i + 1] == ')' || str[i + 1] == '(') && str[i] == '.') {
            printf("2");
            return 0;
        }

        if ((((str[i] >= 'a') && (str[i] <= 'z')) || ((str[i] >= 'A') && (str[i] <= 'Z'))) && there_is_double) {
            printf("3");
            return 0;
        }

        if (str[i + 1] == '.' && there_is_double) {
            printf("4");
            return 0;
        }

        if ((str[i + 1] == '+' || str[i + 1] == '-' || str[i + 1] == '*' || str[i + 1] == '/' || str[i + 1] == ')' || str[i + 1] == '(') && (str[i] >= '0' && str[i] <= '9') && there_is_double) {
            there_is_double = 0;
        }

        if (str[i] >= '0' && str[i] <= '9' && str[i + 1] == '.' && !there_is_double) {
            there_is_double = 1;
        }
    }

    return 1;
}



//основаня идея состоит в создании двух стеков: один содержит операнды, другой- операции
//обрабатывая фрагмент списка без скобок(!), все элементы кладутся в стек в соответствии со своим предназначением
//после того
void calcucateListWithoutBrackets(list_of_symb** L, number** C) {
    if ((*L)->elem->imp_char == '-') {
        makeNegNumber(&((*L)->next->elem->operand));
        list_of_symb* T = (*L)->next;
        free(*L);
        (*L) = T;
    }

    if ( (*L) -> next  == NULL){
        (*C)->mul = NULL;
        (*C)->term = (*L) -> elem   -> operand -> term;
        if ( (*L) -> elem   -> operand -> mul != NULL )
            addElemToListMul(&((*C)->mul), (*L) -> elem   -> operand -> mul -> indef);
        return;
    }

    list_of_operands* stack_op = NULL;
    list* stack_ch = NULL;
    list_of_symb* T = *L;

    pushNumber(&stack_op, T->elem->operand);
    push(&stack_ch, T->next->elem->imp_char);
    pushNumber(&stack_op, T->next->next->elem->operand);

    if (T->next->elem->imp_char == '*' || T->next->elem->imp_char == '/') {
        char operation = pop(&stack_ch);

        number* A = (number*)malloc(sizeof(number));
        number* B = (number*)malloc(sizeof(number));
        number* C = (number*)malloc(sizeof(number));

        initNumb(&A);
        initNumb(&B);
        initNumb(&C);

        popNumber(&stack_op, &B);
        popNumber(&stack_op, &A);
        
        if ( (A) -> mul != NULL ) {
            int K= searchValueOfVar(values, (A)->mul->indef);
            if( K!= 0) {
                (A)->term= keyOfValue(values, K);
            }
            else{
                printf("Give me value of %s: ", (A)->mul->indef);
                double d;
                scanf("%lf", &d);
                addElemToVarList(&values, (A)->mul->indef, d);
                (A)->term= d;
            }
        }
        
        if ( (B) -> mul != NULL ) {
            int K= searchValueOfVar(values, (B)->mul->indef);
            if( K!= 0) {
                (B)->term= keyOfValue(values, K);
            }
            else{
                printf("Give me value of %s: ", (B)->mul->indef);
                double d;
                scanf("%lf", &d);
                addElemToVarList(&values, (B)->mul->indef, d);
                (B)->term= d;
            }
        }

        if (operation == '*') {
            mulNumb(&A, &B, &C);
        }
        else if (operation == '/') {
            divNumb(&A, &B, &C);
        } else {
            printf("/nFATAL ERROR/n");
            exit(1);
        }

        pushNumber(&stack_op, C);

        free(A);
        free(B);
        free(C);

    }

    if ( T -> next -> next ->  next != NULL ){
        T = T-> next -> next -> next;

        while ( T != NULL ){
            push(&stack_ch, T->elem->imp_char);
            pushNumber(&stack_op, T->next->elem->operand);

            if (T->elem->imp_char == '*' || T->elem->imp_char == '/') {

                char operation = pop(&stack_ch);

                number* A = (number*)malloc(sizeof(number));
                number* B = (number*)malloc(sizeof(number));
                number* D = (number*)malloc(sizeof(number));
                
                initNumb(&A);
                initNumb(&B);
                initNumb(&D);

                popNumber(&stack_op, &B);
                popNumber(&stack_op, &A);
                
                if ( (A) -> mul != NULL ) {
                    int K= searchValueOfVar(values, (A)->mul->indef);
                    if( K!= 0) {
                        (A)->term= keyOfValue(values, K);
                    }
                    else{
                        printf("Give me value of %s: ", (A)->mul->indef);
                        double d;
                        scanf("%lf", &d);
                        addElemToVarList(&values, (A)->mul->indef, d);
                        (A)->term= d;
                    }
                }
                
                if ( (B) -> mul != NULL ) {
                    int K= searchValueOfVar(values, (B)->mul->indef);
                    if( K!= 0) {
                        (B)->term= keyOfValue(values, K);
                    }
                    else{
                        printf("Give me value of %s: ", (B)->mul->indef);
                        double d;
                        scanf("%lf", &d);
                        addElemToVarList(&values, (B)->mul->indef, d);
                        (B)->term= d;
                    }
                }

                if (operation == '*') {
                    mulNumb(&A, &B, &D);
                }
                else if (operation == '/') {
                    divNumb(&A, &B, &D);
                } else {
                    printf("/nFATAL ERROR/n");
                    exit(1);
                }

                pushNumber(&stack_op, D);

                free(A);
                free(B);
                free(D);

            }

            T= T->next->next;

        }

    }


    while (stack_op->next != NULL) {
    //на данном этапе было необоходимо превратить стек в очередь
        char operation = popFromQue(&stack_ch);
        
        number* A = (number*)malloc(sizeof(number));
        number* B = (number*)malloc(sizeof(number));
        number* D = (number*)malloc(sizeof(number));

        initNumb(&A);
        initNumb(&B);
        initNumb(&D);

        popFromQueueNumber(&stack_op, &A);
        popFromQueueNumber(&stack_op, &B);
        
        if ( (A) -> mul != NULL ) {
            int K= searchValueOfVar(values, (A)->mul->indef);
            if( K!= 0) {
                (A)->term= keyOfValue(values, K);
            }
            else{
                printf("Give me value of %s: ", (A)->mul->indef);
                double d;
                scanf("%lf", &d);
                addElemToVarList(&values, (A)->mul->indef, d);
                (A)->term= d;
            }
        }
        
        if ( (B) -> mul != NULL ) {
            int K= searchValueOfVar(values, (B)->mul->indef);
            if( K!= 0) {
                (B)->term= keyOfValue(values, K);
            }
            else{
                printf("Give me value of %s: ", (B)->mul->indef);
                double d;
                scanf("%lf", &d);
                addElemToVarList(&values, (B)->mul->indef, d);
                (B)->term= d;
            }
        }

        if (operation == '+') {
            addNumbers(&A, &B, &D);
        }
        else {
            difNumb(&A, &B, &D);
        }

        pushToQueueNumber(&stack_op, D);

        free(A);
        free(B);
        free(D);
    }
    
    (*C)->mul = NULL;
    (*C)->term = stack_op->elem->term;
    if ( stack_op->elem->mul != NULL )
        addElemToListMul(&((*C)->mul), stack_op->elem->mul->indef);
    return;
}

int containsOpenBrackets( list_of_symb* L ){
    while ( L != NULL ){
        if ( L -> elem -> imp_char == '(')
            return 1;
        L= L -> next;
    }
    return 0;
}

void deleteForNewElemInsteadBracket(list_of_symb**L, number** C){
    if ( *C == NULL )
        return;

    if ( (*L) -> elem -> imp_char == '(' && !containsOpenBrackets((*L)->next) ){
        list_of_symb* T = NULL;

        while ( (*L)-> next -> elem -> imp_char != ')' ){

            T= (*L)-> next-> next;
            if ( ((*L)->next->elem->operand) != NULL ){
                deleteNumb(&((*L)->next->elem->operand));
                (*L)->next->elem->operand= NULL;
            }
            free((*L)->next);
            (*L)->next= T;

        }
        T= T->next;
        if ( ((*L)->next->elem->operand) != NULL ){
            deleteNumb(&((*L)->next->elem->operand));
            (*L)->next->elem->operand= NULL;
        }
        free((*L)->next);
        (*L)->next= T;

        (*L) -> elem -> imp_char = 0;
        (*L) -> elem -> operand = (number*)malloc(sizeof(number));
        initNumb(&((*L)->elem->operand));

        (*L)->elem->operand->term= (*C)->term;
        if ( (*C)->mul != NULL)
            addElemToListMul(&((*L)->elem->operand->mul), (*C)->mul->indef);
    }
    else{
        deleteForNewElemInsteadBracket(&((*L)->next), C);
    }
}

void deleteSymbListElem(list_of_symb** L){
    if ( (*L)->next == NULL ){
        if ( (*L)->elem->imp_char != 0 ){
            free(*L);
            *L= NULL;
        }
        else{
            if ( (*L) -> elem -> operand != NULL){
                deleteNumb(&( (*L) -> elem -> operand ));
            }
            else{
                printf("\nError22\n");
            }
            free(*L);
            *L= NULL;
        }
    }
    else{
        deleteSymbListElem(&((*L)->next));
    }
}

void deleteSymbList( list_of_symb** L ){
    while ( *L != NULL ){
        deleteSymbListElem(L);
    }
}

void calculateWithBrackets(list_of_symb** L, number** C){
    list_of_symb* T= *L;

    while (containsOpenBrackets(*L)) {
        if ( T -> elem -> imp_char == '('  && !containsOpenBrackets(T->next)){
            T = T->next;
            int boolean= 0;
            list_of_symb* inside_L= NULL;
            symb* S= NULL;

            while ( T -> elem -> imp_char != ')' ){
                boolean= 1;

                if ( T -> elem -> imp_char != '\0' ){
                    S= (symb*)malloc(sizeof(symb));
                    S->imp_char= T -> elem -> imp_char;
                    S->operand= NULL;
                    addSymb(&inside_L, S);
                    free(S);
                    S= NULL;
                }
                else if ( T -> elem -> operand != NULL ){
                    S= (symb*)malloc(sizeof(symb));
                    S->operand= (number*)malloc(sizeof(number));
                    S->imp_char= 0;

                    S->operand->mul=NULL;
                    S->operand->term= T -> elem -> operand -> term;

                    if ( T -> elem -> operand -> mul != NULL )
                        addElemToListMul(&( S->operand->mul ), T -> elem -> operand -> mul -> indef);
                    
                    addSymb(&inside_L, S);

                    free(S->operand);
                    S->operand= NULL;
                    free(S);
                }
                T= T-> next;
            }

            if( boolean ){
                number* Temp= (number*)malloc(sizeof(number));
                initNumb(&Temp);
                calcucateListWithoutBrackets(&inside_L, &Temp);
                deleteForNewElemInsteadBracket(L, &Temp);
                deleteSymbList(&inside_L);
                deleteNumb(&Temp);
                free(Temp);

                T= *L;
            }
            else{
                printf("\nERROR BRACKETS\n");
            }
        }
        else{
            T= T->next;
        }

    }

    calcucateListWithoutBrackets(L, C);

}

int main(void) {
    int error[5] = { 0 };
    char* calculating_str = NULL;
    int num_of_sym = 0;
    char letter;

    //Ввод строки и настаивание на исправление ошибок
    //Излишние скобки не являются ошибочными, если возможна интерпретация записи, не отходя от общепринятых правил
    //Пользователь будет получать просьбу о вводе до тех пор пока не введет задачу без ошибок или не нажмет ENTER

    while (!multiplyArray(error, 5)) {
        free(calculating_str);
        calculating_str = NULL;
        num_of_sym = 0;

        printf("Input your task and press ENTER. Or press ENTER, if you do not want to give me task\n");

        scanf("%c", &letter);
        while (letter != '\n') {
            num_of_sym++;
            if (num_of_sym == 1) {
                calculating_str = (char*)malloc(sizeof(char));
            }
            else {
                calculating_str = (char*)realloc(calculating_str, num_of_sym * sizeof(char));
            }
            calculating_str[num_of_sym - 1] = letter;
            scanf("%c", &letter);
        }
        calculating_str = (char*)realloc(calculating_str, (num_of_sym + 1) * sizeof(char));
        calculating_str[num_of_sym] = 0;

        error[0] = checkBrackets(calculating_str);
        error[1] = checkMissingOperand(calculating_str);
        error[2] = checkPassOfOperation(calculating_str);
        error[3] = checkInvalidOperation(calculating_str);
        error[4] = checkRigthOperands(calculating_str);

        for (int i = 0; i < 5; i++) {
            if (!error[i]) {
                if (i == 0)
                    printf("Error: out of balance between brackets\n");
                
                if (i == 1)
                    printf("Error: possibly missing operand\n");
                
                if (i == 2)
                    printf("Error: possible some operation is passed\n");

                if (i == 3)
                    printf("Error: invalid operation\n");

                if (i == 4)
                    printf("Error: possibly constants are not right\n");
                
            }
        }

    }

    if (calculating_str[0] == 0) {
        printf("There is not task. Bye\n");
    }

    else {
        list_of_symb* list_symb = NULL;
        list_symb = NULL;
        strToList(calculating_str, &list_symb);
        number* C = (number*)malloc(sizeof(number));
        calculateWithBrackets(&list_symb, &C);
        if ( C-> mul != NULL ){
            int K= searchValueOfVar(values, (C)->mul->indef);
            if( K!= 0) {
                (C)->term= keyOfValue(values, K);
            }
            else{
                printf("Give me value of %s: ", (C)->mul->indef);
                double d;
                scanf("%lf", &d);
                addElemToVarList(&values, (C)->mul->indef, d);
                (C)->term= d;
            }
        }
        printf("ANSWER: ");
        printf("%lf\n", C->term);
        deleteNumb(&C);
        deleteVarList(&values);
        values= NULL;
    }

    return 0;
}
