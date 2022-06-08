#include <curl/curl.h>
#include <json/json.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>

std::size_t callback(
    const char *in,
    std::size_t size,
    std::size_t num,
    std::string *out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

class NameAndValue
{
public:
    std::string name;
    double value;
    NameAndValue(const char *name, double value) : name(name), value(value) {}
};

std::vector<NameAndValue> curlURL()
{
    std::vector<NameAndValue> response;

    const char *url = "https://www.cbr-xml-daily.ru/latest.js";

    CURL *curl = curl_easy_init();

    // Set remote URL.
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Response information.
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (httpCode == 200)
    {
        std::cout << "\nGot successful response from " << url << std::endl;

        // Response looks good - done using Curl now.  Try to parse the results
        // and print them out.
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            // std::cout << "Successfully parsed JSON data" << std::endl;
            // std::cout << "\nJSON data received:" << std::endl;
            // std::cout << jsonData.toStyledString() << std::endl;

            Json::Value valute = jsonData["rates"];
            for (auto i : valute.getMemberNames())
            {
                response.push_back(NameAndValue(i.c_str(), valute[i].asDouble()));
            }
        }
        else
        {
            std::cout << "Could not parse HTTP data as JSON" << std::endl;
            std::cout << "HTTP data was:\n"
                      << *httpData.get() << std::endl;
        }
    }
    else
    {
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
    }

    return response;
}

int main(int argc, char const *argv[])
{

    std::map<std::string, std::vector<double>> map;
    for (int loop = 0; loop < 10000000; loop++)
    {
        auto start = std::chrono::system_clock::now();
        auto arr = curlURL();
        for (auto i : arr)
        {
            std::cout << i.name << "  " << i.value << '\n';
            map[i.name].push_back(i.value);
        }

        for (std::map<std::string, std::vector<double>>::iterator
                 it = map.begin();
             it != map.end(); ++it)
        {
            auto vector = it->second;
            double mediana = vector[vector.size() / 2];
            double sum = 0;
            for (int i = 0; i < vector.size(); i++)
                sum += vector[i];

            std::cout << it->first << ": mediana: " << mediana << ", srednany: " << sum / vector.size() << std::endl;
        }

        auto end = std::chrono::system_clock::now();
        auto duration = end - start;
        std::this_thread::sleep_for(std::chrono::seconds(10) - duration);
       }

    return 0;
}
