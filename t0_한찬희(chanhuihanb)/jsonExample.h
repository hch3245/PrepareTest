#pragma once

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <format>

class MyClass {
public:
    std::string name;
    int value;

    // MyClass 객체를 JSON 객체로 변환하는 함수
    json::JSON ToJSON() const {
        json::JSON j = json::JSON::Make(json::JSON::Class::Object);
        j["name"] = name;    // 템플릿 대입 연산자가 동작하도록, 문자열이 JSON으로 변환됨
        j["value"] = value;  // 템플릿 대입 연산자로 정수형이 JSON으로 변환됨
        return j;
    }

    // JSON 객체에서 MyClass 객체를 복원하는 함수
    void FromJSON(const json::JSON& j) {
        // at()을 사용하여 키로 값을 얻고, ToString(), ToInt() 함수를 사용하여 원래 타입으로 변환
        name = j.at("name").ToString();
        value = j.at("value").ToInt();
    }
};

// MyClass 객체를 파일에 저장하는 함수
void SaveMyClassToFile(const MyClass& obj, const std::string& filename) {
    // MyClass를 JSON 객체로 변환
    json::JSON j = obj.ToJSON();
    // JSON 객체를 문자열로 변환 (pretty printing은 dump()의 구현에 따라 다를 수 있음)
    std::string jsonStr = j.dump();

    // 파일 스트림을 열어서 jsonStr을 저장
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
        return;
    }
    ofs << jsonStr;
    ofs.close();
}

// 파일에서 MyClass 객체를 불러오는 함수
MyClass LoadMyClassFromFile(const std::string& filename) {
    // 파일 스트림을 열어서 전체 내용을 읽어들임
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Error: Cannot open file " << filename << " for reading." << std::endl;
        return MyClass();
    }
    std::string jsonStr((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    // JSON::Load를 사용해 JSON 문자열을 파싱하여 JSON 객체 생성
    json::JSON j = json::JSON::Load(jsonStr);
    MyClass obj;
    obj.FromJSON(j);
    return obj;
}