// sudo apt-get install libcurl4-openssl-dev

#include <curl/curl.h>
#include "yyjson.h"

void getVal(char* out){

    yyjson_mut_doc* m_mut_doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* m_mut_root = yyjson_mut_obj(m_mut_doc);
    yyjson_mut_doc_set_root(m_mut_doc, m_mut_root);

    yyjson_mut_obj_add_str(m_mut_doc, m_mut_root, "messageId", "messageId1");
	yyjson_mut_obj_add_str(m_mut_doc, m_mut_root, "happenTime", "happenTime2");
    yyjson_mut_obj_add_int(m_mut_doc, m_mut_root, "isValid", 1);
    yyjson_mut_obj_add_int(m_mut_doc, m_mut_root, "dealTime", 1234);
    yyjson_mut_obj_add_str(m_mut_doc, m_mut_root, "dealContent", "dealContent2");
    yyjson_mut_obj_add_str(m_mut_doc, m_mut_root, "relatedEventid", "relatedEventid5");

    const char* json_ = yyjson_mut_write(m_mut_doc, 0, NULL);
	if (json_)
	{
		memcpy(out, json_, 1024);
		free((void*)json_);
	}

    yyjson_mut_doc_free(m_mut_doc);


}

int main(){

    char out[1024] = {0};
    getVal(out);

    CURL* curl = NULL;
    CURLcode resCode;

    curl = curl_easy_init();

    struct curl_slist *headers = NULL;

    while(1){

        if(curl){

            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.225.128:20006/service/receiveDetectorStatus");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type:application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, out);

            resCode = curl_easy_perform(curl); // 推送数据
            if(resCode != CURLE_OK){
                printf("推送失败\n");
            }else{
                printf("推送成功\n");
            }
        }

        sleep(2);
    }
   
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return 0;
}

