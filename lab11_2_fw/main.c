#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 歌曲节点结构体
typedef struct Song {
    int id;
    char title[100];
    char artist[50];
    char filepath[300];
    struct Song* prev;
    struct Song* next;
} Song;

// 播放列表管理器
typedef struct PlaylistManager{
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
int delete_song_by_title(PlaylistManager* manager, const char* title);                   // 删除指定名字的音乐
int play_song_by_title(PlaylistManager* manager, const char* title);                     // 根据名字播放音乐
void display_playlist(PlaylistManager* manager);                                // 显示播放列表（正向）
void display_playlist_reverse(PlaylistManager* manager);                        // 显示播放列表（反向）
int export_playlist(PlaylistManager* manager, const char* filename);            // 导出歌单
void next_song(PlaylistManager* manager);                                       // 下一首歌
void previous_song(PlaylistManager* manager);                                   // 上一首歌
int play_song_random(PlaylistManager* manager);                                 // 随机播放音乐（非必做）
int insert_song_at(PlaylistManager* manager, int position, const char* title, const char* artist, const char* filepath);    // 向指定位置添加音乐（非必做）
void clear_playlist(PlaylistManager* manager);                                  // 清空播放列表（非必做）
void sort_by_title(PlaylistManager* manager);                                   // 按照歌曲名排序（非必做）

// 跨平台播放：Windows / macOS / Linux
// 说明：
// 1) 先用 fopen 检查文件是否存在（存在则立即 fclose）
// 2) 再用 system() 调用系统默认方式播放/打开
// 3) 若系统命令失败，退化为“模拟播放”打印提示（保证实验可验收）
void play_audio(const char* filename)
{
    if (filename == NULL || filename[0] == '\0') {
        printf("文件名为空，无法播放。\n");
        return;
    }

    // 先检查文件是否能打开（存在性/权限），避免 system 白跑
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("无法打开文件：%s\n", filename);
        return;
    }
    fclose(fp);  // 立刻关闭，避免句柄泄露

    // 组装命令（注意给路径加引号，兼容空格/中文）
    char command[512];

#ifdef _WIN32
    // Windows：使用 start 调用默认关联播放器
    // 说明：start 的第一个参数是窗口标题，需给空字符串 ""，否则带空格路径会被误解析
    snprintf(command, sizeof(command), "start \"\" \"%s\"", filename);
#elif defined(__APPLE__)
    // macOS：优先用 afplay（音频播放），如果你更想用默认播放器，可改成 open
    // snprintf(command, sizeof(command), "open \"%s\"", filename);
    snprintf(command, sizeof(command), "afplay \"%s\"", filename);
#else
    // Linux：用 xdg-open 调用默认关联程序（桌面环境一般都有）
    snprintf(command, sizeof(command), "xdg-open \"%s\"", filename);
#endif

    int ret = system(command);

    // ret != 0 代表命令执行失败或被中断（不同平台含义略有差异，这里统一视为失败）
    if (ret != 0) {
        printf("系统播放器调用失败（ret=%d），改为模拟播放：%s\n", ret, filename);
    }
}


// 初始化播放管理器
void init_playlist_manager(PlaylistManager* manager){
    manager->head = NULL;
    manager->tail = NULL;
    manager->current = NULL;
    manager->song_count = 0;
}

// 从文件读取内容构建双向链表
int load_songs_from_file(PlaylistManager* manager, const char* filename) {
    if (manager == NULL || filename == NULL) return 0;

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("无法打开歌单文件 %s（将以空歌单启动）\n", filename);
        return 0;
    }

    char line[1024];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 去除行尾换行符（兼容 \r\n）
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;

        char* title = strtok(line, ",");
        char* artist = strtok(NULL, ",");
        char* filepath = strtok(NULL, "");

        if (!title || !artist || !filepath) continue;

        // trim
        while (*title == ' ' || *title == '\t') title++;
        while (*artist == ' ' || *artist == '\t') artist++;
        while (*filepath == ' ' || *filepath == '\t') filepath++;

        size_t n = strlen(title);
        while (n > 0 && (title[n-1] == ' ' || title[n-1] == '\t')) title[--n] = '\0';

        n = strlen(artist);
        while (n > 0 && (artist[n-1] == ' ' || artist[n-1] == '\t')) artist[--n] = '\0';

        n = strlen(filepath);
        while (n > 0 && (filepath[n-1] == ' ' || filepath[n-1] == '\t')) filepath[--n] = '\0';

        // 追加到链表尾部（加载时不输出“添加成功”）
        Song* newSong = (Song*)malloc(sizeof(Song));
        if (!newSong) {
            printf("内存分配失败，停止加载。\n");
            break;
        }

        newSong->id = (manager->tail != NULL) ? manager->tail->id + 1 : 1;

        strncpy(newSong->title, title, sizeof(newSong->title) - 1);
        newSong->title[sizeof(newSong->title) - 1] = '\0';

        strncpy(newSong->artist, artist, sizeof(newSong->artist) - 1);
        newSong->artist[sizeof(newSong->artist) - 1] = '\0';

        strncpy(newSong->filepath, filepath, sizeof(newSong->filepath) - 1);
        newSong->filepath[sizeof(newSong->filepath) - 1] = '\0';

        /* TODO:
         * 将 newSong 尾插到“双向链表”
         * TIP:
         * - newSong 的 prev 应该指向当前 tail；newSong 的 next 应为 NULL
         * - 空链表：head 和 tail 都指向 newSong
         * - 非空链表：原 tail->next = newSong，newSong->prev = 原 tail，再更新 tail
         * - 注意维护 head/tail 的一致性
         */
        if (manager->tail == NULL) {
            manager->head = newSong;
            manager->tail = newSong;
            newSong->prev = NULL;
            newSong->next = NULL;
        } 
        else {
            manager->tail->next = newSong;
            newSong->prev = manager->tail;
            newSong->next = NULL;
            manager->tail = newSong;
        }
        manager->song_count++;
        loaded++;
    }

    fclose(fp);

    if (manager->current == NULL) {
        manager->current = manager->head;
    }
    return loaded;
}

// 1. 在链表末尾添加歌曲
void add_song(PlaylistManager* manager, const char* title, const char* artist, const char* filepath) {
    if (manager == NULL || title == NULL || artist == NULL || filepath == NULL) {
        printf("参数错误，添加失败。\n");
        return;
    }

    Song* newSong = (Song*)malloc(sizeof(Song));
    if (!newSong) {
        printf("内存分配失败，添加失败。\n");
        return;
    }

    newSong->id = (manager->tail != NULL) ? manager->tail->id + 1 : 1;

    strncpy(newSong->title, title, sizeof(newSong->title) - 1);
    newSong->title[sizeof(newSong->title) - 1] = '\0';

    strncpy(newSong->artist, artist, sizeof(newSong->artist) - 1);
    newSong->artist[sizeof(newSong->artist) - 1] = '\0';

    strncpy(newSong->filepath, filepath, sizeof(newSong->filepath) - 1);
    newSong->filepath[sizeof(newSong->filepath) - 1] = '\0';

    /*
    TODO
    Tips：
    1) 双向链表尾插 = 改 2~3 个关键指针：原 tail->next，新节点->prev，最后更新 tail
    2) 空表时不能写 manager->tail->next（tail 为空会崩），要先让 head/tail 指向新节点
    3) 新尾结点的 next 必须是 NULL；否则正向遍历会越界/死循环
    4) 别忘了更新 song_count；很多“显示为空/数量不对”都是因为漏了这一句
    5) current 的策略：若原来为空，建议指向新歌；若已有 current，可不改（你的设计里空表时设 current 更合理）
    */
    if (manager->tail == NULL) {
        manager->head = newSong;
        manager->tail = newSong;
        newSong->prev = NULL;
        newSong->next = NULL;
    } 
    else {
        manager->tail->next = newSong;
        newSong->prev = manager->tail;
        newSong->next = NULL;
        manager->tail = newSong;
    }
    if (manager->current == NULL) {
        manager->current = newSong;
    }
    manager->song_count++;
    printf("已添加：%s - %s\n", newSong->title, newSong->artist);
}



// 2. 按标题删除歌曲（删除所有同名歌曲）
int delete_song_by_title(PlaylistManager* manager, const char* title) {
    if (manager == NULL || title == NULL) return 0;

    if (manager->head == NULL) {
        printf("播放列表为空，无需删除。\n");
        return 0;
    }

    int deleted = 0;
    Song* cur = manager->head;

    while (cur != NULL) {
        Song* next = cur->next;

        if (strcmp(cur->title, title) == 0)
        {
            /* TODO：删除当前节点 cur
                   1）如果 cur 正好是 current，先调整 current
                   2）将 cur 从双向链表中断开（注意头结点、尾结点情况）
                   3）释放 cur，更新 song_count 和 deleted
                */
            if (cur == manager->current) {
                if (cur->next != NULL) {
                    manager->current = cur->next;
                } 
                else if (cur->prev != NULL) {
                    manager->current = cur->prev;
                } 
                else {
                    manager->current = NULL;
                }
            }
            if (cur->prev != NULL) {
                cur->prev->next = cur->next;
            } 
            else {
                manager->head = cur->next;
            }

            if (cur->next != NULL) {
                cur->next->prev = cur->prev;
            } 
            else {
                manager->tail = cur->prev;
            }

            free(cur);
            manager->song_count--;
            deleted++;
        }
        cur = next;
    }

    if (manager->song_count == 0) {
        manager->head = manager->tail = manager->current = NULL;
    } 
    else if (manager->current == NULL) {
        manager->current = manager->head;
    }

    if (deleted == 0) printf("未找到标题为 \"%s\" 的歌曲。\n", title);
    else printf("已删除 %d 首标题为 \"%s\" 的歌曲。\n", deleted, title);

    return deleted;
}


// 3. 播放歌曲
int play_song_by_title(PlaylistManager* manager, const char* title){
    if (manager == NULL || title == NULL) return 0;

    Song* p = manager->head;
    while (p != NULL) {
        if (strcmp(p->title, title) == 0) {
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


// 4. 显示播放列表（正向遍历）
void display_playlist(PlaylistManager* manager) {
    if (manager == NULL || manager->head == NULL) {
        printf("播放列表为空。\n");
        return;
    }

    printf("\n===== 播放列表（正向，共 %d 首）=====\n", manager->song_count);
    printf("%-4s | %-30s | %-20s | %s\n", "ID", "Title", "Artist", "Filepath");
    printf("--------------------------------------------------------------------------\n");

    /* TODO：正向遍历双向链表并输出每一首歌曲
       - 从 head 开始
       - 使用 next 指针向后移动
    */
    Song* p = manager->head;
    while (p != NULL) {
        printf("%-4d | %-30s | %-20s | %s\n", p->id, p->title, p->artist, p->filepath);
        p = p->next;
    }
}

// 5. 显示播放列表（逆向遍历）
void display_playlist_reverse(PlaylistManager* manager) {
    if (manager == NULL || manager->tail == NULL) {
        printf("播放列表为空。\n");
        return;
    }

    printf("\n===== 播放列表（逆向，共 %d 首）=====\n", manager->song_count);
    printf("%-4s | %-30s | %-20s | %s\n", "ID", "Title", "Artist", "Filepath");
    printf("--------------------------------------------------------------------------\n");


    /* TODO：逆向遍历双向链表并输出每一首歌曲
       - 从 tail 开始
       - 使用 prev 指针向前移动
    */
    Song* p = manager->tail;
    while (p != NULL) {
        printf("%-4d | %-30s | %-20s | %s\n", p->id, p->title, p->artist, p->filepath);
        p = p->prev;
    }
}

// 6. 将播放列表保存到文件
int export_playlist(PlaylistManager* manager, const char* filename) {
    if (manager == NULL || filename == NULL) return 0;

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("无法写入文件 %s\n", filename);
        return 0;
    }

    int count = 0;

    /* TODO：将链表中的歌曲按顺序写入文件
       - 从 head 开始遍历双向链表
       - 每个节点写一行：title,artist,filepath
       - 统计写入的歌曲数量 count
    */
    Song* p = manager->head;
    while (p != NULL) {
        fprintf(fp, "%s,%s,%s\n", p->title, p->artist, p->filepath);
        count++;
        p = p->next;
    }

    fclose(fp);
    printf("已导出 %d 首歌曲到 %s\n", count, filename);
    return 1;
}

// 7. 下一首
void next_song(PlaylistManager* manager) {
    if (manager == NULL || manager->head == NULL) {
        printf("播放列表为空。\n");
        return;
    }

    /* TODO：切换到下一首歌曲
       - 如果 current 为空，从 head 开始播放
       - 如果 current 后面还有歌曲，移动到 next
       - 如果已经是最后一首，回到 head（循环播放）
    */
    if (manager->current == NULL) {
        manager->current = manager->head;
    } 
    else if (manager->current->next != NULL) {
        manager->current = manager->current->next;
    } 
    else {
        manager->current = manager->head;
    }

    printf("切换到下一首：%s - %s\n", manager->current->title, manager->current->artist);
    play_audio(manager->current->filepath);
}


// 8. 上一首
void previous_song(PlaylistManager* manager) {
    if (manager == NULL || manager->head == NULL) {
        printf("播放列表为空。\n");
        return;
    }

    /* TODO：切换到上一首歌曲
       - 如果 current 为空，从 tail 开始播放
       - 如果 current 前面还有歌曲，移动到 prev
       - 如果已经是第一首，回到 tail（循环播放）
    */
    if (manager->current == NULL) {
        manager->current = manager->tail;
    } 
    else if (manager->current->prev != NULL) {
        manager->current = manager->current->prev;
    } 
    else {
        manager->current = manager->tail;
    }

    printf("切换到上一首：%s - %s\n", manager->current->title, manager->current->artist);
    play_audio(manager->current->filepath);
}


// 9. 随机播放歌曲（非必做）
int play_song_random(PlaylistManager* manager) {
    if (manager == NULL) return 0;
    if (manager->song_count <= 0 || manager->head == NULL) {
        printf("播放列表为空，无法随机播放。\n");
        return 0;
    }

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }

    int index = rand() % manager->song_count;

    /* TODO：根据随机下标选择一首歌曲
       - 从 head 开始
       - 沿 next 指针向后移动 index 次
       - 若意外走到 NULL，可回到 head
       - 将 current 指向选中的歌曲
    */
    Song* p = manager->head;
    for (int i = 0; i < index; i++) {
        if (p->next != NULL) {
            p = p->next;
        } 
        else {
            p = manager->head;
        }
    }
    manager->current = p;

    printf("随机播放：%s - %s\n", manager->current->title, manager->current->artist);
    play_audio(manager->current->filepath);
    return 1;
}

// 10. 在指定位置插入歌曲（非必做）
// position 采用 1-based：1 表示插入到表头；song_count+1 表示追加到表尾。
// 若 position <= 1，则插入表头；若 position > song_count，则插入表尾。
int insert_song_at(PlaylistManager* manager, int position, const char* title,
                   const char* artist, const char* filepath) {
    if (manager == NULL || title == NULL || artist == NULL || filepath == NULL) {
        printf("参数错误，插入失败。\n");
        return 0;
    }

    Song* newSong = (Song*)malloc(sizeof(Song));
    if (!newSong) {
        printf("内存分配失败，插入失败。\n");
        return 0;
    }

    newSong->id = (manager->tail != NULL) ? manager->tail->id + 1 : 1;

    strncpy(newSong->title, title, sizeof(newSong->title) - 1);
    newSong->title[sizeof(newSong->title) - 1] = '\0';

    strncpy(newSong->artist, artist, sizeof(newSong->artist) - 1);
    newSong->artist[sizeof(newSong->artist) - 1] = '\0';

    strncpy(newSong->filepath, filepath, sizeof(newSong->filepath) - 1);
    newSong->filepath[sizeof(newSong->filepath) - 1] = '\0';

        /* TODO：在指定位置插入新节点
       - 初始化新节点的 prev / next
       - 处理空表情况
       - position <= 1：插入表头
       - position > song_count：插入表尾
       - 否则：插入到中间位置
       - 最后维护 song_count 和 current
    */
    if (manager->head == NULL) {
        manager->head = newSong;
        manager->tail = newSong;
        newSong->prev = NULL;
        newSong->next = NULL;
    } 
    else if (position <= 1) {
        newSong->next = manager->head;
        newSong->prev = NULL;
        manager->head->prev = newSong;
        manager->head = newSong;
    } 
    else if (position > manager->song_count) {
        newSong->prev = manager->tail;
        newSong->next = NULL;
        manager->tail->next = newSong;
        manager->tail = newSong;
    } 
    else {
        Song* p = manager->head;
        for (int i = 1; i < position - 1; i++) {
            p = p->next;
        }
        newSong->next = p->next;
        newSong->prev = p;
        p->next->prev = newSong;
        p->next = newSong;
    }
    manager->song_count++;
    manager->current = newSong;

    printf("已插入到位置 %d：%s - %s\n", position, newSong->title, newSong->artist);
    return 1;
}

// 12. 按歌曲标题排序（非必做）
void sort_by_title(PlaylistManager* manager) {
    if (manager == NULL || manager->head == NULL || manager->head->next == NULL) {
        printf("无需排序（歌曲数量不足）。\n");
        return;
    }

    /* TODO：按歌曲标题对链表进行排序
       - 最后更新 head 和 tail
    */
    int swapped;
    do {
        swapped = 0;
        Song* p = manager->head;

        while (p->next != NULL) {
            if (strcmp(p->title, p->next->title) > 0) {
                // 交换节点 p 和 p->next
                Song* q = p->next;

                // 调整前驱节点
                if (p->prev != NULL) {
                    p->prev->next = q;
                } else {
                    manager->head = q; // 更新 head
                }
                q->prev = p->prev;

                // 调整后继节点
                p->next = q->next;
                if (q->next != NULL) {
                    q->next->prev = p;
                } else {
                    manager->tail = p; // 更新 tail
                }
                q->next = p;
                p->prev = q;

                swapped = 1;
            } else {
                p = p->next;
            }
        }
    } while (swapped);
    printf("已按标题排序。\n");
}


// 销毁整个链表
void clear_playlist(PlaylistManager* manager) {
    Song* current = manager->head;
    while (current != NULL) {
        Song* next = current->next;
        free(current);
        current = next;
    }
    init_playlist_manager(manager);
    printf("播放列表已清空\n");
}


void display_menu() {
    printf("\n");
    printf("链表音乐播放器管理器\n");
    printf("==========================================\n");
    printf("1. 添加歌曲\n");
    printf("2. 删除歌曲 (按标题)\n");
    printf("3. 播放歌曲 (按标题)\n");
    printf("4. 显示播放列表 (正向)\n");
    printf("5. 显示播放列表 (逆向)\n");
    printf("6. 导出歌单\n");
    printf("7. 切换到下一首歌\n");
    printf("8. 切换到上一首歌\n");
    printf("9. 随机播放歌曲(非必做)\n");
    printf("10. 在指定位置添加歌曲(非必做)\n");
    printf("11. 清空播放列表(非必做)\n");
    printf("12. 按照歌曲名排序(非必做)\n");
    printf("0. 退出程序\n");
    printf("==========================================\n");
    printf("请选择操作 (0-12): ");
}

// 清除输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 获取用户输入的字符串
void get_user_input(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);

    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }
}

// 主函数 - 交互式程序
int main() {
    PlaylistManager manager;
    init_playlist_manager(&manager);
    load_songs_from_file(&manager,"song_list.txt");

    printf("=== 双向链表音乐播放器管理器 ===\n");
    printf("已加载 %d 首示例歌曲\n", manager.song_count);
    manager.current = manager.head;
    int choice;

    do {
        display_menu();

        if (scanf("%d", &choice) != 1) {
            printf("无效输入，请输入数字\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: {                   // 添加歌曲
                char title[100], artist[50], filepath[300];
                float duration;

                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                clear_input_buffer();

                add_song(&manager, title, artist, filepath);
                break;
            }
            case 2: {                   // 删除歌曲 (按标题)
                char title[100];
                get_user_input(title, sizeof(title), "请输入要删除的歌曲标题: ");
                int res = delete_song_by_title(&manager, title);
                break;
            }
            case 3: {                   // 播放歌曲（按歌曲名）
                char title[100];
                get_user_input(title, sizeof(title), "请输入要播放的歌曲标题: ");
                int res = play_song_by_title(&manager, title);
                break;
            }
            case 4: {                   // 显示播放列表（正向）
                display_playlist(&manager);
                break;
            }
            case 5: {                   // 显示播放列表（逆向）
                display_playlist_reverse(&manager);
                break;
            }
            case 6: {                   // 导出播放列表
                char path2export[300];
                get_user_input(path2export, sizeof(path2export), "请输入要导出的目标文件名: ");
                int res = export_playlist(&manager, path2export);
                break;
            }
            case 7: {                   // 播放下一首歌曲
                next_song(&manager);
                break;
            }
            case 8: {                   // 播放上一首歌曲
                previous_song(&manager);
                break;
            }
            case 9: {                   // 随机播放歌曲(非必做)
                int res = play_song_random(&manager);
                break;
            }
            case 10: {                  // 在指定位置添加歌曲(非必做)
                char title[100], artist[50], filepath[300];
                int position;
                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                printf("请输入歌曲插入位置: ");
                scanf("%d", &position);
                insert_song_at(&manager, position, title, artist, filepath);
                break;
            }
            case 11: {                  // 清空播放列表(非必做)
                clear_playlist(&manager);
                break;
            }
            case 12: {                  // 按照歌曲名排序(非必做)
                sort_by_title(&manager);
                break;
            }
            case 0: // 退出程序
                printf("感谢使用链表音乐播放器管理器!\n");
                break;
            default:
                printf("? 无效选择，请重新输入\n");
                break;
        }

        // 暂停，让用户看到结果
        if (choice != 0) {
            printf("\n按回车键继续...");
            getchar();
        }

    } while (choice != 0);

    // 清理内存
    clear_playlist(&manager);

    return 0;
}
