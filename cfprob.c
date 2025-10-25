#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

const char *tags[] = {
  "implementation",
  "math",
  "greedy",
  "dp",
  "data+structures",
  "brute+force",
  "constructive+algorithms",
  "graphs",
  "sortings",
  "binary+search",
  "dfs+and+similar",
  "trees",
  "strings",
  "number+theory",
  "combinatorics",
  "geometry",
  "bitmasks",
  "two+pointers",
  "dsu",
  "shortest+paths",
  "probabilities",
  "divide+and+conquer",
  "hashing",
  "games",
  "flows",
  "interactive",
  "matrices",
  "string+suffix+structures",
  "fft",
  "graph+matchings",
  "ternary+search",
  "expression+parsing",
  "meet-in-the-middle",
  "2-sat",
  "chinese+remainder+theorem",
  "schedules"
};

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s);

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);

void findLink(char *tag, int targRating);

void blacklist(char *probLink);

void removeFromBlacklist(char *probLink);

void showBlacklist();

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "[!] missing arguments\n");
    puts("[*] try -h for help");
    exit(1);
  }

  if (strlen(argv[1]) != 2) {
    fprintf(stderr, "[!] invalid flag\n");
    puts("[*] try -h for help");
    exit(1);
  }

  if (argv[1][0] != '-') {
    fprintf(stderr, "[!] invalid flag\n");
    puts("[*] try -h for help");
    exit(1);
  }

  switch (argv[1][1]) {
    case 'h':
      puts("valid flags:");
      puts("-h\tdisplay this help menu");
      puts("-f\tfind problem link, supply args: {tag} {rating}");
      puts("-b\tblacklist problem link, supply args: {valid_problem_link}");
      puts("-r\tremove problem link from blacklist, supply args: {valid problem limk}");
      puts("-v\tview blacklist");

      puts("\nvalid tags:");
      for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        printf("%s\n", tags[i]);
      
      puts("\nvalid ratings:");
      for (int i = 8; i <= 32; i++)
        printf("%d\n", i * 100);

      break;

    case 'f':
      if (argc != 4) {
        fprintf(stderr, "[!] missing/invalid arguments\n");
        puts("[*] try -h for help");
        exit(1);
      }

      char tag[128]; strncpy(tag, argv[2], 128);
      int rating = atoi(argv[3]);

      int found = 0;
      for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
        if (!strcmp(tag, tags[i])) { found = 1; break; }
      }

      if (!found) {
        fprintf(stderr, "[!] invalid problem tag\n");
        puts("[*] try -h for help");
        exit(1);
      }

      if (!(rating % 100 == 0 && 8 <= rating / 100 && rating / 100 <= 32)) {
        fprintf(stderr, "[!] invalid rating\n");
        puts("[*] try -h for help");
        exit(1);
      }

      findLink(tag, rating);

      break;

    case 'b':
      if (argc != 3) {
        fprintf(stderr, "[!] missing/invalid arguments");
        puts("try -h for help");
        exit(1);
      }
      blacklist(argv[2]);
      puts("[+] blacklist updated");
      break;

    case 'r':
      if (argc != 3) {
        fprintf(stderr, "[!] missing/invalid arguments");
        puts("try -h for help");
        exit(1);
      }
      removeFromBlacklist(argv[2]);
      puts("[+] blacklist updated");
      break;
    
    case 'v':
      if (argc != 2) {
        fprintf(stderr, "[!] missing/invalid arguments\n");
        puts("try -h for help");
        exit(1);
      }
      showBlacklist();
      break;

    default:
      fprintf(stderr, "[!] invalid flag\n");
      puts("[*] try -h for help");
      exit(1);
  }

  return 0;
}

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(1);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(1);
  }
  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;
  return size * nmemb;
}

void findLink(char *tag, int targRating) {
  char apiLink[128];
  snprintf(apiLink, 128, "https://codeforces.com/api/problemset.problems?tags=%s", tag);

  puts("[*] initializing curl...");

  CURL *curl = curl_easy_init();
  CURLcode res;

  if (curl == NULL) {
    fprintf(stderr, "[!] failed to initialize curl\n");
    exit(1);
  }

  puts("[+] curl successfully initialized!");

  struct string s;
  init_string(&s);

  puts("[*] processing api call...");

  curl_easy_setopt(curl, CURLOPT_URL, "https://codeforces.com/api/problemset.problems?tags=data+structures");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
  res = curl_easy_perform(curl);

  if (res != CURLE_OK)
    fprintf(stderr, "error: %s", curl_easy_strerror(res));
  
  curl_easy_cleanup(curl);

  puts("[+] api call successful!");
  puts("[*] parsing JSON response...");

  cJSON *probs = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_Parse(s.ptr), "result"), "problems");
  cJSON *prob, *rating, *contestId, *index;

  cJSON_ArrayForEach(prob, probs) {
    rating = cJSON_GetObjectItem(prob, "rating");
    contestId = cJSON_GetObjectItem(prob, "contestId");
    index = cJSON_GetObjectItem(prob, "index");
    if (rating == NULL || contestId == NULL || index == NULL || rating->valueint != targRating)
      continue;

    char probLink[128];
    snprintf(probLink, 128, "https://codeforces.com/problemset/problem/%d/%s", contestId->valueint, index->valuestring);

    puts("[*] scanning blacklisted links...");

    FILE *fptr = fopen("blacklist", "r");
    if (fptr == NULL) {
      fprintf(stderr, "[!] failed to check blacklist\n");
      return;
    }

    // char buf[128]; int blacklisted = 0, i = 0;
    // while ((buf[i] = fgetc(fptr)) != EOF) {
    //   if (buf[i] == '\n') {
    //     buf[i] = '\0'; i = -1;
    //     if (!strcmp(probLink, buf)) {
    //       blacklisted = 1; break;
    //     }
    //   }
    //   i++;
    // }

    char buf[128]; int blacklisted = 0;
    while (fgets(buf, 128, fptr) != NULL) {
      buf[sizeof(buf) - 1] = '\0';
      if (!strcmp(buf, probLink)) {
        blacklisted = 1; break;
      }
    }

    fclose(fptr);
    if (blacklisted) continue;

    printf("[+] found problem link!\n%s\n", probLink);
    break;
  };

  free(s.ptr);
}

void blacklist(char *probLink) {
  FILE *fptr = fopen("blacklist", "a");
  if (fptr == NULL) {
    fprintf(stderr, "[!] failed to open blacklist\n");
    exit(1);
  }
  fprintf(fptr, "%s\n", probLink);
  fclose(fptr);
}

void removeFromBlacklist(char *probLink) {
  FILE *fptr = fopen("blacklist", "r+");
  if (fptr == NULL) {
    fprintf(stderr, "[!] failed to open blacklist\n");
    exit(1);
  }

  struct string newbuf;
  init_string(&newbuf);
  char buf[128]; int len, found = 0;
  while (fgets(buf, 128, fptr) != NULL) {
    len = strlen(buf); buf[len - 1] ='\0';
    if (!strcmp(buf, probLink)) { found = 1; continue; }
    buf[len - 1] = '\n';
    writefunc(buf, 1, len, &newbuf);
  }

  printf("%s\n", newbuf); // fix this, sometimes prints garbage at the end

  if (!found)
    fprintf(stderr, "[!] problem link not in blacklist\n");
}

void showBlacklist() {
  FILE *fptr = fopen("blacklist", "r");
  if (fptr == NULL) {
    fprintf(stderr , "[!] failed to open blacklist\n");
    exit(1);
  }

  // get size of file
  fseek(fptr, 0, SEEK_END);
  int sz = ftell(fptr);
  rewind(fptr);

  char *buf = malloc(sz);
  fread(buf, 1, sz, fptr);
  buf[sz] = '\0';

  puts("blacklisted links:");
  printf("%s\n", buf);
}