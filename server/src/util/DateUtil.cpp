#include "util/DateUtil.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

namespace DateUtil {

string hoje() {
    time_t t = time(nullptr);
    return paraString(t);
}

time_t paraTimeT(const string& dataIso) {
    tm tm{};
    istringstream ss(dataIso);
    ss >> get_time(&tm, "%Y-%m-%d");
    tm.tm_hour = 12; // poe meio dia pra nao dar problema com fuso
    return mktime(&tm);
}

string paraString(time_t t) {
    tm* tm = localtime(&t);
    ostringstream oss;
    oss << put_time(tm, "%Y-%m-%d");
    return oss.str();
}

int diferencaEmDias(const string& data1, const string& data2) {
    time_t t1 = paraTimeT(data1);
    time_t t2 = paraTimeT(data2);
    double segundos = difftime(t2, t1);
    return (int)(round(segundos / (60 * 60 * 24)));
}

string somarMeses(const string& dataIso, int meses) {
    tm tm{};
    istringstream ss(dataIso);
    ss >> get_time(&tm, "%Y-%m-%d");
    tm.tm_hour = 12;

    int totalMeses = tm.tm_mon + meses;
    tm.tm_year += totalMeses / 12;
    tm.tm_mon = totalMeses % 12;
    if (tm.tm_mon < 0) {
        tm.tm_mon += 12;
        tm.tm_year -= 1;
    }

    time_t t = mktime(&tm);
    return paraString(t);
}

}
