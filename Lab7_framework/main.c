#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>    // Sleep()
#else
#include <unistd.h>     // sleep()
#endif

#define MAX_RECORDS 100
#define SCORE_FILE  "score.txt"

/* 一个成绩记录 */
typedef struct
{
    char name[20];      // 玩家姓名（不含空格）
    int  ms;            // 反应时间（毫秒）
    char date[11];      // 日期：YYYY-MM-DD 字符串（10+1）
    char time_str[9];   // 时间：HH:MM:SS 字符串（8+1）
} Record;

/* 函数声明 */
void clear_input_buffer(void);
int  load_records(Record rec[], int max_count);
void show_records(const Record rec[], int count);
int  play_game(void);
int  add_record(Record rec[], int *count, int ms);
void wait_random_seconds(int min_s, int max_s);

int main(void)
{
    Record records[MAX_RECORDS];
    int count;
    int choice;

    // 随机数种子，用于随机等待时间
    srand((unsigned int)time(NULL));

    // 1. 程序启动时，从 score.txt 中读取历史成绩
    count = load_records(records, MAX_RECORDS);
    printf("Loaded %d records from %s.\n\n", count, SCORE_FILE);

    // 2. 菜单循环
    while (1)
    {
        printf("---------- Reaction Time Test ----------\n");
        printf("1. Start game\n");
        printf("2. Show records\n");
        printf("0. Quit\n");
        printf("Please choose: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input, please try again.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();   // 吃掉行尾的回车

        if (choice == 0)
        {
            printf("Bye.\n");
            break;
        }

        if (choice == 1)
        {
            // 3. 进行一次游戏，得到反应时间 T（毫秒）
            int ms = play_game();
            if (ms < 0)
            {
                printf("Game failed, please try again.\n");
                continue;
            }

            printf("Save this record? (y/n): ");
            int ch = getchar();
            clear_input_buffer();

            if (ch == 'y' || ch == 'Y')
            {
                // 4. 追加记录到内存和文件中
                if (add_record(records, &count, ms) == 1)
                    printf("Record saved.\n");
                else
                    printf("Failed to save record.\n");
            }
        }
        else if (choice == 2)
        {
            // 5. 查看成绩列表
            show_records(records, count);
        }
        else
        {
            printf("Invalid choice, please try again.\n");
        }

        printf("\n");
    }

    return 0;
}

/* 清空输入缓冲区，防止后续 getchar 读到多余回车 */
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        /* 丢弃多余字符 */
    }
}

/* 随机等待 min_s ~ max_s 秒 */
void wait_random_seconds(int min_s, int max_s)
{
    int delay = rand() % (max_s - min_s + 1) + min_s;
#ifdef _WIN32
    Sleep(delay * 1000);
#else
    sleep(delay);
#endif
}

/* 一次游戏：随机等待 + 计时
   返回反应时间（毫秒），如果计时异常则返回 -1 */
int play_game(void)
{
    int ch;
    clock_t start, end;
    double elapsed_ms;

    printf("\n(Waiting randomly, please do NOT press anything...)\n");
    wait_random_seconds(1, 3);

    printf("Now press Enter!\n");
    fflush(stdout);   // 确保提示立即输出

    // 开始计时
    start = clock();

    // 等待玩家按下回车，只关心 '\n'
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        // 什么都不做，只是丢弃其他按键
    }

    end = clock();

    elapsed_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Your reaction time: %.0f ms.\n", elapsed_ms);

    if (elapsed_ms < 0)
    {
        return -1;
    }
    return (int)(elapsed_ms + 0.5);  // 四舍五入
}

/* =========================================================
   TODO-1: load_records

   功能：
     从成绩文件 SCORE_FILE（文本格式）中读取历史记录，
     填充到数组 rec 中，并返回成功读取的数量。

   参数：
     rec       - 记录数组（输出）
     max_count - 数组容量上限

   返回值：
     成功读取的条数；0 表示没有记录/文件不存在。
   文件格式：每行
     name ms YYYY-MM-DD HH:MM:SS
   ========================================================= */
int load_records(Record rec[], int max_count)
{
    FILE *fp;
    int count = 0;

    fp = fopen(SCORE_FILE, "r");
    if (fp == NULL)
    {
        return 0;
    }
    while (count < max_count)
    {
        int ret = fscanf(fp, "%19s %d %10s %8s",
                         rec[count].name,
                         &rec[count].ms,
                         rec[count].date,
                         rec[count].time_str);
        if (ret == 4)
        {
            count++;
        }
        else
        {
            break;
        }
    }

    fclose(fp);
    return count;
}

/* =========================================================
   TODO-2: show_records

   功能：
     以表格形式在屏幕上输出成绩记录，
     每条显示编号、姓名、反应时间（毫秒）、记录日期和时间。

   参数：
     rec   - 记录数组；
     count - 有效记录条数。

   返回值：
     无（void）
   ========================================================= */
void show_records(const Record rec[], int count)
{
    int i;

    if (count <= 0)
    {
        printf("\nNo records.\n");
        return;
    }

    printf("\n----------- Records -----------\n");
    printf("%-4s %-20s %-10s %-10s %-8s\n",
           "No", "Name", "Time(ms)", "Date", "Time");
    printf("-----------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        printf("%-4d %-20s %-10d %-10s %-8s\n",
               i + 1,
               rec[i].name,
               rec[i].ms,
               rec[i].date,
               rec[i].time_str);
    }

    printf("-----------------------------------------\n");
}

/* =========================================================
   add_record

   功能：
     在数组尾部追加一条新记录，并把这条记录追加写入 score.txt。

   参数：
     rec[]   - 记录数组（输入 / 输出）
     count   - 当前记录数量指针（输入 / 输出）
     ms      - 本次反应时间（毫秒）

   返回值：
     1   表示成功追加 1 条记录
    -1   表示失败（数组满、输入无效或文件写失败）
   ========================================================= */
int add_record(Record rec[], int *count, int ms)
{
    FILE *fp;
    Record r;
    time_t now;
    struct tm *t;

    if (*count >= MAX_RECORDS)
    {
        printf("Record list is full, cannot save.\n");
        return -1;
    }

    printf("Please enter your name (no spaces, up to 19 chars): ");
    if (scanf("%19s", r.name) != 1)
    {
        printf("Invalid name.\n");
        clear_input_buffer();
        return -1;
    }
    clear_input_buffer();

    now = time(NULL);
    t = localtime(&now);
    if (t != NULL)
    {
        sprintf(r.date, "%04d-%02d-%02d",
                t->tm_year + 1900,
                t->tm_mon + 1,
                t->tm_mday);

        sprintf(r.time_str, "%02d:%02d:%02d",
                t->tm_hour,
                t->tm_min,
                t->tm_sec);
    }
    else
    {
        strcpy(r.date, "0000-00-00");
        strcpy(r.time_str, "00:00:00");
    }

    r.ms = ms;

    fp = fopen(SCORE_FILE, "a");
    if (fp == NULL)
    {
        printf("Failed to open %s for writing.\n", SCORE_FILE);
        return -1;
    }

    if (fprintf(fp, "%s %d %s %s\n",
                r.name, r.ms, r.date, r.time_str) < 0)
    {
        printf("Failed to write to %s.\n", SCORE_FILE);
        fclose(fp);
        return -1;
    }

    fclose(fp);

    rec[*count] = r;
    (*count)++;

    return 1;
}