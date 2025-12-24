#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 歌曲节点结构体
typedef struct Song
{
    int id;
    char title[100];
    char artist[50];
    char filepath[300];
    struct Song* next;
} Song;

// 播放列表管理器
typedef struct PlaylistManager
{
    Song* head;
    Song* tail;
    Song* current;
    int song_count;
} PlaylistManager;

// 函数声明
void init_playlist_manager(PlaylistManager* manager);                           // 初始化链表
int load_songs_from_file(PlaylistManager* manager, const char* filename);       // 从文件中读取到链表
void add_song(PlaylistManager* manager, const char* title, const char* artist,  // 人工增加音乐
              const char* filepath);
void display_playlist(PlaylistManager* manager);                                // 显示播放列表
int delete_songs_by_title(PlaylistManager* manager, const char* title);         // 删除指定名字的音乐
int play_song_by_title(PlaylistManager* manager, const char* title);            // 根据名字播放音乐
int export_playlist(PlaylistManager* manager, const char* filename);            // 导出播放列表
int play_song_random(PlaylistManager* manager);                                 // 随机播放音乐
int insert_song_at(PlaylistManager* manager, int position, const char* title, const char* artist, const char* filepath);    // 向指定位置添加音乐
void destroy_playlist(PlaylistManager* manager);                                // 清空列表

// linux/Mac 版本
// void play_audio(const char* filename) {
//     char command[256];
//     FILE *mp3File = fopen(filename, "rb");
//     if (!mp3File) {
//         printf("无法打开文件 %s\n", filename);
//         return;
//     }
//     else{
//         printf("Founded File!!");
//     }
//     snprintf(command, sizeof(command), "afplay \"%s\"", filename);
//     int ret = system(command);
//     if (ret != 0) {
//         printf("播放失败或中断，请检查文件格式是否支持。\n");
//     }
// }

// Windows 版本
void play_audio(const char* filename)
{
    char command[256];
    FILE *mp3File = fopen(filename, "rb");
    if (!mp3File)
    {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    fclose(mp3File);
    snprintf(command, sizeof(command), "start \"\" \"%s\"", filename);
    int ret = system(command);
    if (ret != 0)
    {
        printf("播放失败或中断，请检查文件格式是否支持。\n");
    }

    // 或者使用 Windows Media Player
    // sprintf(command, "wmplayer \"%s\"", filename);
    // system(command);
}

int load_songs_from_file(PlaylistManager* manager, const char* filename)
{
    // 功能：从文本文件中读取歌曲信息，构建单链表歌单
    // 文件格式：title,artist,filepath（一行一首歌）
    // 返回值：成功加载的歌曲数量

    // 参数合法性检查
    if (manager == NULL || filename == NULL) return 0;

    // 以只读方式打开文件
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        // 文件不存在或无法打开时，不视为致命错误，直接使用空歌单
        printf("无法打开歌单文件 %s（将以空歌单启动）\n", filename);
        return 0;
    }

    char line[1024];   // 用于存放每一行文本
    int loaded = 0;    // 统计成功加载的歌曲数量

    // 逐行读取文件内容
    while (fgets(line, sizeof(line), fp) != NULL)
    {

        // ---------- 1. 去除行尾的换行符 ----------
        // 同时兼容 Windows(\r\n) 和 Unix(\n) 换行格式
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[len - 1] = '\0';
            len--;
        }
        // 空行直接跳过
        if (len == 0) continue;

        // ---------- 2. 解析一行数据 ----------
        // 使用 strtok 按逗号分割：title,artist,filepath
        char* title = strtok(line, ",");
        char* artist = strtok(NULL, ",");
        // 第三个字段使用 strtok(NULL, "")，表示“剩余所有内容”
        // 这样可以避免路径中出现特殊字符时被误截断
        char* filepath = strtok(NULL, "");

        // 如果字段不完整，跳过该行
        if (!title || !artist || !filepath)
        {
            continue;
        }

        // ---------- 3. 去除字段首尾的空白字符 ----------
        // strtok 返回的是 line 内部的指针，可以直接原地修改
        {
            // 去除 title 首尾空白
            while (*title == ' ' || *title == '\t') title++;
            size_t n = strlen(title);
            while (n > 0 && (title[n-1] == ' ' || title[n-1] == '\t'))
                title[--n] = '\0';

            // 去除 artist 首尾空白
            while (*artist == ' ' || *artist == '\t') artist++;
            n = strlen(artist);
            while (n > 0 && (artist[n-1] == ' ' || artist[n-1] == '\t'))
                artist[--n] = '\0';

            // 去除 filepath 首尾空白
            while (*filepath == ' ' || *filepath == '\t') filepath++;
            n = strlen(filepath);
            while (n > 0 && (filepath[n-1] == ' ' || filepath[n-1] == '\t'))
                filepath[--n] = '\0';
        }

        // ---------- 4. 创建新歌曲节点 ----------
        // 加载阶段采用“尾插法”，避免频繁遍历链表
        Song* newSong = (Song*)malloc(sizeof(Song));
        if (!newSong)
        {
            // 内存不足，直接停止加载，保留已加载的内容
            printf("内存分配失败，停止加载。\n");
            break;
        }

        // 为歌曲分配一个自增 id（仅用于链表内部管理，不写回文件）
        newSong->id = (manager->tail != NULL) ? manager->tail->id + 1 : 1;

        // 拷贝字符串到结构体成员中，确保不会发生越界
        strncpy(newSong->title, title, sizeof(newSong->title) - 1);
        newSong->title[sizeof(newSong->title) - 1] = '\0';

        strncpy(newSong->artist, artist, sizeof(newSong->artist) - 1);
        newSong->artist[sizeof(newSong->artist) - 1] = '\0';

        strncpy(newSong->filepath, filepath, sizeof(newSong->filepath) - 1);
        newSong->filepath[sizeof(newSong->filepath) - 1] = '\0';

        newSong->next = NULL;

        // ---------- 5. 插入到链表尾部 ----------
        if (manager->head == NULL)
        {
            // 空链表：新节点既是头结点也是尾结点
            manager->head = manager->tail = newSong;
        }
        else
        {
            manager->tail->next = newSong;
            manager->tail = newSong;
        }

        manager->song_count++;
        loaded++;
    }

    // 文件使用完毕，及时关闭
    fclose(fp);

    // 如果当前播放指针为空，默认指向第一首歌
    if (manager->current == NULL)
    {
        manager->current = manager->head;
    }

    return loaded;
}



// 初始化播放管理器
void init_playlist_manager(PlaylistManager* manager)
{
    manager->head = NULL;
    manager->tail = NULL;
    manager->current = NULL;
    manager->song_count = 0;
}

// 1. 在链表末尾添加歌曲
void add_song(PlaylistManager* manager, const char* title, const char* artist,
              const char* filepath)
{
    if (manager == NULL || title == NULL || artist == NULL || filepath == NULL)
    {
        printf("参数错误，添加失败。\n");
        return;
    }

    Song* newSong = (Song*)malloc(sizeof(Song));
    if (!newSong)
    {
        printf("内存分配失败，添加失败。\n");
        return;
    }

    /* TODO: 初始化 newSong 的数据域 */
    // TIP:
    //  - 给歌曲分配一个 id（通常在原尾结点 id 的基础上 +1）
    //  - 将 title / artist / filepath 拷贝到结构体中
    //  - 注意字符串结尾要保证有 '\0'
    //  - 新节点的 next 指针此时应指向 NULL
    newSong -> id = (manager -> tail != NULL) ? manager -> tail -> id + 1 : 1;
    strncpy(newSong -> title, title, sizeof(newSong -> title) - 1);
    newSong -> title[sizeof(newSong -> title) - 1] = '\0';
    strncpy(newSong -> artist, artist, sizeof(newSong -> artist) - 1);
    newSong -> artist[sizeof(newSong -> artist) - 1] = '\0';
    strncpy(newSong -> filepath, filepath, sizeof(newSong -> filepath) - 1);
    newSong -> filepath[sizeof(newSong -> filepath) - 1] = '\0';
    newSong -> next = NULL;


    /* TODO: 将 newSong 插入链表尾部 */
    // 将新节点插入到链表尾部
    // TIP:
    //  - 需要区分“空链表”和“非空链表”两种情况
    //  - 空链表：head 和 tail 都应指向 newSong
    //  - 非空链表：原 tail->next 指向 newSong，再更新 tail
    //  - 插入完成后，记得更新歌曲总数 song_count
    //  - 可根据需要更新 current 指针（例如：第一次添加歌曲时）
    if (manager -> head == NULL) {
        manager -> head = newSong;
        manager -> tail = newSong;
        manager -> current = newSong;
    }
    else {
        manager -> tail -> next = newSong;
        manager -> tail = newSong; 
    }
    manager -> song_count++;

    printf("已添加：%s - %s\n", newSong->title, newSong->artist);

}

// 2. 显示播放列表
void display_playlist(PlaylistManager* manager)
{
    if (manager == NULL || manager->head == NULL)
    {
        printf("播放列表为空。\n");
        return;
    }

    printf("\n===== 播放列表（共 %d 首）=====\n", manager->song_count);
    printf("%-4s | %-30s | %-20s | %s\n", "ID", "Title", "Artist", "Filepath");
    printf("--------------------------------------------------------------------------\n");

    // 定义一个指针，从头结点开始遍历
    Song* p = manager->head;

    /* TODO: 使用 while 循环遍历链表并输出每首歌 */
    // TIP:
    //  - 当指针不为 NULL 时，说明还有节点
    //  - 每次循环应先处理当前节点，再移动到下一个节点
    while (p != NULL) {
        printf("%-4d | %-30s | %-20s | %s\n", p -> id, p -> title, p -> artist, p -> filepath);
        p = p -> next;
    }

}

// 3. 删除歌曲
// 删除最早出现的标题为 title 的歌曲，返回成功返回 1，否则返回 0
int delete_songs_by_title(PlaylistManager* manager, const char* title)
{
    if (manager == NULL || title == NULL) return 0;

    if (manager->head == NULL)
    {
        printf("播放列表为空，无需删除。\n");
        return 0;
    }

    int deleted = 0;
    Song* prev = NULL;
    Song* cur = manager->head;

    while (cur != NULL)
    {
        if (strcmp(cur->title, title) == 0)
        {

            /* TODO: 删除逻辑 */
            // 删除当前节点 cur
            // TIP:
            //  - 需要保存当前要删除的节点指针
            //  - 删除后要让 cur 指向下一个节点
            //  - 区分删除的是头结点(prev==NULL)还是中间/尾结点
            //  - 记得释放被删除节点的内存
            //  - 更新歌曲数量和 deleted 计数
            Song* toDelete = cur;
            cur = cur -> next;
            if (prev == NULL) {
                manager -> head = cur;
            }
            else {
                prev -> next = cur;
            }
            free(toDelete);
            deleted++;
            manager -> song_count--;
        }
        else {
            // 未匹配，向后移动
            prev = cur;
            cur = cur->next;
        }
    }

    if (manager->head == NULL)
    {
        manager->tail = NULL;
        manager->current = NULL;
    }

    if (deleted == 0)
    {
        printf("未找到标题为 \"%s\" 的歌曲。\n", title);
    }
    else
    {
        printf("已删除 %d 首标题为 \"%s\" 的歌曲。\n", deleted, title);
    }
    return deleted;
}

// 4. 播放歌曲
int play_song_by_title(PlaylistManager* manager, const char* title)
{
    if (manager == NULL || title == NULL) return 0;

    Song* p = manager->head;
    while (p != NULL)
    {
        if (strcmp(p->title, title) == 0)
        {
            manager->current = p;
            printf("正在播放：%s - %s\n", p->title, p->artist);
            play_audio(p->filepath);
            return 1;
        }
        p = p->next;
    }

    printf("未找到标题为 \"%s\" 的歌曲。\n", title);
    return 0;
}

// 5. 将播放列表保存到文件
int export_playlist(PlaylistManager* manager, const char* filename)
{
    if (manager == NULL || filename == NULL) return 0;

    FILE* fp = fopen(filename, "w");
    if (!fp)
    {
        printf("无法写入文件 %s\n", filename);
        return 0;
    }

    int count = 0;

    // TODO: 遍历链表，把每首歌写入文件
    // TIP:
    //  - 从 manager->head 开始遍历
    //  - 每个节点写一行：title,artist,filepath
    //  - 行末要加 \n
    //  - 每写一首歌，count 记得 +1
    //  - 指针每次移动到 next，直到 NULL 结束
    Song* curr = manager -> head;
    while (curr != NULL) {
        fprintf(fp, "%s,%s,%s\n", curr -> title, curr -> artist, curr -> filepath);
        count++;
        curr = curr -> next;
    }

    fclose(fp);
    printf("已导出 %d 首歌曲到 %s\n", count, filename);
    return 1;
}

// 6. 随机播放歌曲（非必做）
// position 采用 1-based：1 表示插入到表头；song_count+1 表示追加到表尾。
// 若 position <= 1，则插入表头；若 position > song_count，则插入表尾。
int play_song_random(PlaylistManager* manager)
{
    if (manager == NULL) return 0;
    if (manager->song_count <= 0 || manager->head == NULL)
    {
        printf("播放列表为空，无法随机播放。\n");
        return 0;
    }

    // 生成随机下标 index（范围 0 ~ song_count-1）
    static int seeded = 0;
    if (!seeded)
    {
        srand((unsigned)time(NULL));
        seeded = 1;
    }

    int index = rand() % manager->song_count;

    Song* p = NULL;
    // TODO: 根据随机下标 index，在链表中找到对应的歌曲节点
    // TIP:
    //  - 从链表头结点 manager->head 开始遍历
    //  - index 表示“从 0 开始计数”的位置（第 0 个是 head）
    //  - 可以使用循环让指针每次向后移动一个节点
    //  - 循环过程中要注意指针是否变为 NULL，避免非法访问
    //  - 如果遍历结束后指针为 NULL，可以回退到 head 作为兜底处理
    p = manager -> head;
    for (int i = 0; i < index; i++) {
        if (p -> next != NULL) {
            p = p -> next;
        }
        else {
            break;
        }
    }

    if (p == NULL) {
        p = manager -> head;
    }

    manager->current = p;
    printf("随机播放：%s - %s\n", p->title, p->artist);
    play_audio(p->filepath);
    return 1;
}

// 7. 在指定位置插入歌曲（非必做）
// position 采用 1-based：1 表示插入到表头；2 表示插入后成为第 2 个节点；song_count+1 表示追加到表尾。
// 若 position <= 1，则插入表头；若 position > song_count，则插入表尾。
int insert_song_at(PlaylistManager* manager, int position, const char* title,
                   const char* artist, const char* filepath)
{
    if (manager == NULL || title == NULL || artist == NULL || filepath == NULL)
    {
        printf("参数错误，插入失败。\n");
        return 0;
    }

    Song* newSong = (Song*)malloc(sizeof(Song));
    if (!newSong)
    {
        printf("内存分配失败，插入失败。\n");
        return 0;
    }

    // id 仅用于链表内部管理，采用自增方式
    newSong->id = (manager->tail != NULL) ? manager->tail->id + 1 : 1;
    /* TODO:
     * 在这里完成新节点 newSong 的初始化 + 插入链表 + 收尾更新。
     * 需要做的事情包括：
     * - 将 title / artist / filepath 复制到 newSong 的对应成员中（注意长度限制，并保证以 '\0' 结尾）；
     * - 将 newSong->next 初始化为 NULL；
     * - 如果当前链表为空（manager->head == NULL），新节点应同时作为 head 和 tail，
     *   并根据需要设置 current 指针、song_count（通常为 1），然后可以直接返回；
     * - 如果链表非空，根据 position 的取值决定插入位置：
     *   position <= 1 插入表头；position > song_count 插入表尾；否则插入到中间（需要先找到前驱节点再调整指针）；
     * - 插入完成后，记得更新歌曲总数 song_count；如 current 为空，可让它指向 head 作为默认值。
     */

    strncpy(newSong -> title, title, sizeof(newSong -> title) - 1);
    newSong -> title[sizeof(newSong -> title) - 1] = '\0';
    strncpy(newSong -> artist, artist, sizeof(newSong -> artist) - 1);
    newSong -> artist[sizeof(newSong -> artist) - 1] = '\0';
    strncpy(newSong -> filepath, filepath, sizeof(newSong -> filepath) - 1);
    newSong -> filepath[sizeof(newSong -> filepath) - 1] = '\0';
    newSong -> next = NULL;
    if (manager -> head == NULL) {
        manager -> head = newSong;
        manager -> tail = newSong;
        manager -> current = newSong;
        manager -> song_count = 1;
        printf("已插入到位置 %d：%s - %s\n", position, newSong->title, newSong->artist);
        return 1;
    }
    if (position <= 1) {
        newSong -> next = manager -> head;
        manager -> head = newSong;
    }
    else if (position > manager -> song_count) {
        manager -> tail -> next = newSong;
        manager -> tail = newSong;
    }
    else {
        Song* prev = manager -> head;
        for (int i = 1; i < position - 1; i++) {
            if (prev -> next != NULL) {
                prev = prev -> next;
            }
        }
        newSong -> next = prev -> next;
        prev -> next = newSong;
    }
    manager -> song_count++;
    if (manager -> current == NULL) {
        manager -> current = manager -> head;
    }
    printf("已插入到位置 %d：%s - %s\n", position, newSong->title, newSong->artist);
    return 1;
}

// 8. 销毁整个链表（非必做）
void destroy_playlist(PlaylistManager* manager)
{
    Song* current = manager->head;
    while (current != NULL)
    {
        // TODO: 释放当前节点并移动到下一个节点
        // TIP:
        //  - 释放节点前，必须先保存 current->next
        //  - free(current) 之后，current 就不能再使用了
        //  - 最后让 current 指向下一个节点，继续循环
        while (current != NULL) {
            Song* toDelete = current;
            current = current -> next;
            free(toDelete);
        }
    }
    init_playlist_manager(manager);
    printf("播放列表已清空\n");
}

void display_menu()
{
    printf("\n");
    printf("链表音乐播放器管理器\n");
    printf("==========================================\n");
    printf("1. 人工添加歌曲\n");
    printf("2. 显示播放列表\n");
    printf("3. 删除歌曲 (按标题)\n");
    printf("4. 播放歌曲 (按标题)\n");
    printf("5. 导出歌单\n");
    printf("6. 随机播放歌曲(非必做)\n");
    printf("7. 在指定位置添加歌曲(非必做)\n");
    printf("8. 清空播放列表(非必做)\n");
    printf("0. 退出程序\n");
    printf("==========================================\n");
    printf("请选择操作 (0-8): ");
}



// 清除输入缓冲区
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 获取用户输入的字符串
void get_user_input(char* buffer, int size, const char* prompt)
{
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n')
    {
        buffer[len-1] = '\0';
    }
}

// 主函数 - 交互式程序
int main()
{
    PlaylistManager manager;
    init_playlist_manager(&manager);
    load_songs_from_file(&manager,"song_list.txt");

    printf("=== 链表音乐播放器管理器 ===\n");
    printf("已加载 %d 首示例歌曲\n", manager.song_count);

    int choice;

    do
    {
        display_menu();

        if (scanf("%d", &choice) != 1)
        {
            printf("无效输入，请输入数字\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
        case 1:                 // 添加歌曲
        {
            char title[100], artist[50], filepath[300];

            get_user_input(title, sizeof(title), "请输入歌曲标题: ");
            get_user_input(artist, sizeof(artist), "请输入作者: ");
            get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");

            add_song(&manager, title, artist, filepath);
            break;
        }
        case 2:                 // 显示播放列表 (正向)
        {
            display_playlist(&manager);
            break;
        }
        case 3:                 // 删除歌曲 (按标题)
        {
            char title[100];
            get_user_input(title, sizeof(title), "请输入要删除的歌曲标题: ");
            delete_songs_by_title(&manager, title);
            break;
        }
        case 4:                   // 按歌曲名播放歌曲
        {
            char title[100];
            get_user_input(title, sizeof(title), "请输入要播放的歌曲标题: ");
            int res = play_song_by_title(&manager, title);
            break;
        }
        case 5:
        {
            char path2export[300];
            get_user_input(path2export, sizeof(path2export), "请输入要导出的目标文件名: ");
            export_playlist(&manager, path2export);
            break;
        }
        case 6:
        {
            play_song_random(&manager);
            break;
        }
        case 7:
        {
            char title[100], artist[50], filepath[300];
            int position;
            get_user_input(title, sizeof(title), "请输入歌曲标题: ");
            get_user_input(artist, sizeof(artist), "请输入作者: ");
            get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
            printf("请输入歌曲插入位置: ");
            scanf("%d", &position);
            clear_input_buffer();
            insert_song_at(&manager, position, title, artist, filepath);
            break;
        }
        case 8:
        {
            destroy_playlist(&manager);
            break;
        }
        case 0: // 退出程序
            printf("感谢使用链表音乐播放器管理器!\n");
            break;

        default:
            printf("无效选择，请重新输入\n");
            break;
        }

        // 暂停，让用户看到结果
        if (choice != 0)
        {
            printf("\n按回车键继续...");
            getchar();
        }

    }
    while (choice != 0);

    // 清理内存
    destroy_playlist(&manager);

    return 0;
}
