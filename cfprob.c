#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

int main(int argc, char *argv[]) {

  switch (argc) {
    case 2:

    if (strcmp(argv[1], "help")) {
      fprintf(stderr, "[!] invalid argument\n");
      puts("[*] try \"cfprob help\"");
      exit(1);
    }

    puts("<help string>");

    break;
    
    case 3:

    char tag[128]; strncpy(tag, argv[1], 128);
    int rating = atoi(argv[2]);
  
    int found = 0;
    for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
      if (!strcmp(tag, tags[i])) { found = 1; break; }
    }
  
    if (!found) {
      fprintf(stderr, "[!] invalid problem tag\n");
      puts("[*] try \"cfprob help\"");
      exit(1);
    }
  
    if (!(rating % 100 == 0 && 8 <= rating / 100 && rating / 100 <= 32)) {
      fprintf(stderr, "[!] invalid rating\n");
      puts("[*] try \"cfprob help\"");
      exit(1);
    }
  
    findLink(tag, rating);

    break;

    default:
    
    fprintf(stderr, "[!] invalid number of arguments\n");
    puts("[*] try \"cfprob help\"");
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

  cJSON *validProbs = cJSON_CreateArray();

  cJSON_ArrayForEach(prob, probs) {
    rating = cJSON_GetObjectItem(prob, "rating");
    contestId = cJSON_GetObjectItem(prob, "contestId");
    index = cJSON_GetObjectItem(prob, "index");

    if (rating == NULL || contestId == NULL || index == NULL || rating->valueint != targRating)
      continue;

    cJSON_AddItemReferenceToArray(validProbs, prob);
  };

  srand(time(NULL));
  prob = cJSON_GetArrayItem(validProbs, rand() % cJSON_GetArraySize(validProbs));

  printf("[+] found problem link!\nhttps://codeforces.com/problemset/problem/%d/%s\n",
         cJSON_GetObjectItem(prob, "contestId")->valueint,
         cJSON_GetObjectItem(prob, "index")->valuestring);

  free(s.ptr);
}