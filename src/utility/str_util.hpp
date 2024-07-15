#pragma once

static std::vector<std::string> splitString(const std::string& str, const std::string& del)
{
    std::vector<std::string> found{}; 
    int start, end = -1 * del.size();
    do {
        start = end + del.size();
        end = str.find(del, start);
        found.emplace_back(str.substr(start, end - start)); 
    } while (end != -1);
    return found;
}