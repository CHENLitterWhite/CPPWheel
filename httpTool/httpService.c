/*
    用于简单的HTTP接受测试
*/

#include "mongoose.h"
#include "yyjson.h"
#include <stdatomic.h>
#include <signal.h>

#define BINDING_ADDR "http:://0.0.0.0:20006"

static atomic_int quit;
static uint64_t number = 0;

static void handerHttpInfo(const char *in, char *out){

    number++;
    printf("------------------ %ld --------------------\n", number);
    printf("%s\n", in);
    printf("------------------ end --------------------\n");

    
    yyjson_mut_doc* m_mut_doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* m_mut_root = yyjson_mut_obj(m_mut_doc);
    yyjson_mut_doc_set_root(m_mut_doc, m_mut_root);

    yyjson_mut_obj_add_int(m_mut_doc, m_mut_root, "code", 200);
	yyjson_mut_obj_add_str(m_mut_doc, m_mut_root, "msg", "接收成功");

    const char* json_ = yyjson_mut_write(m_mut_doc, 0, NULL);
	if (json_)
	{
		memcpy(out, json_, 1024);
		free((void*)json_);
	}

    yyjson_mut_doc_free(m_mut_doc);

}

static void hander(struct mg_connection* c, int ev, void* ev_data, void* arg){

    struct mg_http_message* hm = (struct mg_http_message*)ev_data;

    if(MG_EV_HTTP_MSG == ev){

        if(mg_http_match_uri(hm, "/service/receiveVideoEventDealInfo")){
            
            if(strstr(hm->method.ptr, "POST")){

                if(hm->body.len == 0){
                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "{%s:%s}", "msg", "链接成功");
                }
                else{
                    
                    int len  = hm->body.len + 1;
					char *body = (void *)malloc(len);
                    memset(body, 0, len);
                    memcpy(body, hm->body.ptr, len - 1);
                    char res[1024] = {0};

                    handerHttpInfo(body, res);

                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "%s", res);
                    free(body);
                }
            }
        }
        else if(mg_http_match_uri(hm, "/service/receiveCameraStatus")){

            if(strstr(hm->method.ptr, "POST")){
                
                if(hm->body.len == 0){
                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "{%s:%s}", "msg", "链接成功");
                }
                else{

                    int len  = hm->body.len + 1;
					char *body = (void *)malloc(len);
                    memset(body, 0, len);
                    memcpy(body, hm->body.ptr, len - 1);
                    char res[1024] = {0};

                    handerHttpInfo(body, res);

                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "%s", res);
                    free(body);
                }
            }
        }
        else if(mg_http_match_uri(hm, "/service/receiveDetectorStatus")){

            if(strstr(hm->method.ptr, "POST")){
                
                if(hm->body.len == 0){
                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "{%s:%s}", "msg", "链接成功");
                }
                else{

                    int len  = hm->body.len + 1;
					char *body = (void *)malloc(len);
                    memset(body, 0, len);
                    memcpy(body, hm->body.ptr, len - 1);
                    char res[1024] = {0};

                    handerHttpInfo(body, res);

                    mg_http_reply(c, 200, "Content - Type: application/json\r\n", "%s", res);
                    free(body);
                }
            }
        }
        

    }

}

// void sigint_handler(int signum) {
   
//    printf("收到信号:%d\n", SIGINT);

//    int q = atomic_load(&quit);
//    if(q == 0){
//     atomic_store(&quit, 1);
//    }

//    exit(0);
// }

int main(void){

    // 捕捉 ctrl + C 信号
    // signal(SIGINT, sigint_handler);

    atomic_store(&quit, 0);

    struct mg_mgr m_mgr;

    mg_mgr_init(&m_mgr);
    mg_http_listen(&m_mgr, BINDING_ADDR, hander, NULL);
    
    int q = atomic_load(&quit);
    while (!q)
	{
		mg_mgr_poll(&m_mgr, 1000);
	}

    mg_mgr_free(&m_mgr);
    
    return 0;
}
