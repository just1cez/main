#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 栈元素：存储括号类型（{/}/[/]）和所在索引（便于报错定位）
typedef struct {
    char bracket;  // 括号字符
    int position;  // 括号在JSON字符串中的位置（从0开始）
} StackElem;

// 栈结构体（动态扩容，避免固定容量限制）
typedef struct {
    StackElem* data;     // 栈数据数组
    int top;             // 栈顶指针（-1表示空栈）
    int capacity;        // 当前栈容量
    int max_capacity;    // 最大容量
} Stack;

// 栈操作结果枚举（增强健壮性）
typedef enum {
    STACK_OK,           // 操作成功
    STACK_EMPTY,        // 栈空（弹栈/取栈顶失败）
    STACK_FULL,         // 栈满（压栈失败）
    STACK_MEM_ERR       // 内存分配失败
} StackResult;

// ----------------------------// 学生需实现的栈接口// ----------------------------
// 1. 初始化栈（指定初始容量和最大容量）
StackResult stackInit(Stack* stack, int init_capacity, int max_capacity){
    // 学生实现：
    // 步骤1：检查init_capacity和max_capacity合法性（init <= max，且均>0）
    if (init_capacity > max_capacity || max_capacity <= 0) {
        return STACK_MEM_ERR;
    }
    // 步骤2：为stack->data分配内存（malloc）
    stack -> data = malloc(sizeof(StackElem) * init_capacity);
    // 步骤3：初始化top=-1，capacity=init_capacity，max_capacity=max_capacity
    stack -> top = -1;
    stack -> capacity = init_capacity;
    stack -> max_capacity = max_capacity;
    // 步骤4：返回对应结果（STACK_OK/STACK_MEM_ERR）
    return STACK_OK;
}

// 2. 压栈（将数据存入栈）
StackResult stackPush(Stack* stack, char bracket, int position){
        // 学生实现：
    // 步骤1：检查栈是否满（top+1 == capacity），若满则尝试扩容（需不超过max_capacity）
    if (stack -> top + 1 == stack -> capacity && stack -> capacity < stack -> max_capacity) {
        stack -> capacity += 1;
    }
    // 步骤2：扩容失败返回STACK_FULL，成功则更新capacity
    if (stack -> top + 1 == stack -> capacity && stack -> capacity >= stack -> max_capacity) {
        return STACK_FULL;
    }
    // 步骤3：栈顶指针+1，存入bracket和position
    stack -> top++;
    stack -> data[stack -> top].bracket = bracket;
    stack -> data[stack -> top].position = position;
    // 步骤4：返回STACK_OK
    return STACK_OK;
}

// 3. 弹栈（取出栈顶元素，通过elem输出）
StackResult stackPop(Stack* stack, StackElem* elem){
    // 学生实现：
    // 步骤1：检查栈是否空（stackIsEmpty），空则返回STACK_EMPTY
    if (stackIsEmpty(stack)) return STACK_EMPTY;
    // 步骤2：将栈顶元素赋值给elem（bracket和position）
    elem -> bracket = stack -> data[stack -> top].bracket;
    elem -> position = stack -> data[stack -> top].position;
    // 步骤3：栈顶指针-1
    stack -> top--;
    // 步骤4：返回STACK_OK
    return STACK_OK;
}
// 4. 查看栈顶
StackResult stackPeek(Stack* stack, StackElem* elem) {
    // 学生实现：
    // 步骤1：检查栈是否空（stackIsEmpty），空则返回STACK_EMPTY
    if (stackIsEmpty(stack)) return STACK_EMPTY;
    // 步骤2：将栈顶元素赋值给elem（bracket和position）
    elem -> bracket = stack -> data[stack -> top].bracket;
    elem -> position = stack -> data[stack -> top].position;
    // 步骤3：栈顶指针-1
    // 步骤4：返回STACK_OK
    return STACK_OK; 
}

// 5. 判空（返回1表示空，0表示非空）
int stackIsEmpty(const Stack* stack){
    // 学生实现：返回stack->top == -1 ? 1 : 0
    return stack -> top == -1 ? 1 : 0;
}

// 6. 销毁栈（释放内存）
void stackDestroy(Stack* stack) {
    // 学生实现：释放stack->data的内存，重置top=-1、capacity=0
    free(stack -> data);
    stack -> data = NULL;
    stack -> top = -1;
    stack -> capacity = 0;
    stack -> max_capacity = 0;
}


// 辅助函数
// 判断是否为左括号
int isLeftBracket(char ch) {
    return ch == '{' || ch == '[';
}
// 判断是否为右括号
int isRightBracket(char ch) {
    return ch == '}' || ch == ']';
}
// 判断括号是否匹配
int isBracketMatch(char left, char right) {
    return (left == '{' && right == '}') || (left == '[' && right == ']');
}


// 栈基础版本实现
int jsonBracketCheckBasic(const char *json_str) {
    // 学生实现：
    // 步骤1：处理边界情况（json_str为NULL或空字符串，返回1）
    // 步骤2：初始化栈（调用stackInit）
    // 步骤3：遍历json_str的每个字符：
    //        - 遇到左括号（{/[）：压栈（stackPush）
    //        - 遇到右括号（}/]）：弹栈并检查匹配（isBracketMatch），不匹配则返回0
    //        - 其他字符（如字母、数字、引号）：跳过
    // 步骤4：遍历结束后，若栈为空则返回1（合法），否则返回0（左括号多余）
    // 步骤5：销毁栈（stackDestroy）
    if (!json_str || *json_str == '\0')
        return 1;

    Stack stack;
    stackInit(&stack, 10, 1000);

    for (int i = 0; json_str[i]; i++) {
        char ch = json_str[i];

        if (isLeftBracket(ch)) {
            stackPush(&stack, ch, i);
        } else if (isRightBracket(ch)) {
            StackElem elem;
            if (stackPop(&stack, &elem) != STACK_OK ||
                !isBracketMatch(elem.bracket, ch)) {
                stackDestroy(&stack);
                return 0;
            }
        }
    }

    int result = stackIsEmpty(&stack);
    stackDestroy(&stack);
    return result;
}

// 栈进阶版本实现
// 学生任务：学习状态管理，理解字符串内的括号不应该参与匹配，同时处理转义字符
int jsonBracketCheckAdvanced(const char *json_str) {
// 学生实现：
// 步骤1：处理边界情况（json_str为NULL或空字符串，返回1）
// 步骤2：初始化栈（调用stackInit）
// 步骤3：初始化状态标记（in_string标记字符串状态，escape标记转义状态）
// 步骤4：遍历json_str的每个字符：
//        - 如果在字符串内（in_string为1）：
//             * 如果在转义状态（escape为1）：取消转义状态，继续
//             * 如果遇到转义字符'\\'：进入转义状态
//             * 如果遇到引号'"'且不在转义状态：退出字符串状态
//             * 字符串内的所有括号都不处理
//        - 如果不在字符串内（in_string为0）：
//             * 如果遇到引号'"'：进入字符串状态
//             * 遇到左括号（{/[）：压栈（stackPush）
//             * 遇到右括号（}/]）：弹栈并检查匹配（isBracketMatch），不匹配则返回0
//             * 其他字符（如字母、数字、冒号等）：跳过
// 步骤5：遍历结束后，检查栈是否为空且不在字符串内，两者都满足则返回1，否则返回0
// 步骤6：销毁栈（stackDestroy）
    if (!json_str || *json_str == '\0')
        return 1;

    Stack stack;
    stackInit(&stack, 10, 1000);

    int in_string = 0;
    int escape = 0;

    for (int i = 0; json_str[i]; i++) {
        char ch = json_str[i];

        if (in_string) {
            if (escape) {
                escape = 0;
            } else if (ch == '\\') {
                escape = 1;
            } else if (ch == '"') {
                in_string = 0;
            }
            continue;
        }

        if (ch == '"') {
            in_string = 1;
        } else if (isLeftBracket(ch)) {
            stackPush(&stack, ch, i);
        } else if (isRightBracket(ch)) {
            StackElem elem;
            if (stackPop(&stack, &elem) != STACK_OK ||
                !isBracketMatch(elem.bracket, ch)) {
                stackDestroy(&stack);
                return 0;
            }
        }
    }

    int result = (!in_string && stackIsEmpty(&stack));
    stackDestroy(&stack);
    return result;
}

int main(){
    char str[100];
    // 输入JSON字符串
    scanf("%s", str); 
    // 栈基础版
    int result1 = jsonBracketCheckBasic(str);
    // 栈进阶版
    int result2 = jsonBracketCheckAdvanced(str);
    // 返回JSON括号匹配结果，基础版只能处理简单情况，进阶版总能返回正确答案
    printf("%d %d", result1,result2);
}