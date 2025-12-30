#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 辅助函数：去掉首尾空格及两端的双引号
static char *trim_and_unquote(char *s) {
    char *p = s;
    // 去掉开头的空格
    while (*p && isspace((unsigned char)*p)) p++;
    
    // 去掉结尾的空格
    char *end = p + strlen(p) - 1;
    while (end > p && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    // 如果首尾有双引号，去掉它们
    if (*p == '"' && *end == '"' && end > p) {
        *end = '\0';
        p++;
    }
    return p;
}

static int strieq(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

int main(int argc, char **argv) {
    int IsDebug = 0;

    // 1. 检查环境变量
    char *env = getenv("IsDebug");
    if (env && (strieq(env, "1") || strieq(env, "true"))) IsDebug = 1;

    // 2. 检查命令行参数
    for (int i = 1; i < argc; ++i) {
        if (strieq(argv[i], "--debug") || strieq(argv[i], "-d") || strieq(argv[i], "debug")) {
            IsDebug = 1; break;
        }
        if (strncmp(argv[i], "IsDebug=", 8) == 0) {
            char *v = argv[i] + 8;
            if (strieq(v, "1") || strieq(v, "true")) { IsDebug = 1; break; }
        }
    }

    // 默认值
    char Dist_Server[256] = "am-dist.AkiACG.com";
    char IP[64] = "172.16.8.48";
    char Port[32] = "3502";
    char AppPath[512] = ".\\main.exe";

    // 3. 解析 INI
    if (IsDebug) {
        FILE *f = fopen("AkiTools.ini", "r");
        if (f) {
            char line[1024];
            int inDebugSection = 0;
            while (fgets(line, sizeof(line), f)) {
                char *ln = trim_and_unquote(line);

                // 过滤空行和多种注释 (;, #, //)
                if (ln[0] == '\0' || ln[0] == ';' || ln[0] == '#' || (ln[0] == '/' && ln[1] == '/')) 
                    continue;

                // 识别 Section
                if (ln[0] == '[') {
                    if (strieq(ln, "[Debug]")) inDebugSection = 1;
                    else inDebugSection = 0;
                    continue;
                }

                // 在 [Debug] 块内解析
                if (inDebugSection) {
                    char *eq = strchr(ln, '=');
                    if (!eq) continue;

                    *eq = '\0';
                    char *key = trim_and_unquote(ln);
                    char *val = trim_and_unquote(eq + 1);

                    if (strieq(key, "Dist-Server") || strieq(key, "Dist_Server")) {
                        strncpy(Dist_Server, val, sizeof(Dist_Server)-1);
                    } else if (strieq(key, "IP")) {
                        strncpy(IP, val, sizeof(IP)-1);
                    } else if (strieq(key, "Port")) {
                        strncpy(Port, val, sizeof(Port)-1);
                    } else if (strieq(key, "AppPath")) {
                        strncpy(AppPath, val, sizeof(AppPath)-1);
                    }
                }
            }
            fclose(f);
        }
    }

    // 输出结果
    printf("--- Configuration ---\n");
    printf("IsDebug     : %d\n", IsDebug);
    printf("Dist-Server : %s\n", Dist_Server);
    printf("IP          : %s\n", IP);
    printf("Port        : %s\n", Port);
    printf("AppPath     : %s\n", AppPath);

    return 0;
}