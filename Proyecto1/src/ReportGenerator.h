#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "DataStructures.h"
#include "Token.h"
#include <map>

class ReportGenerator {
public:
    static std::string generatePatientReport(const HospitalData* data);
    static std::string generateDoctorWorkloadReport(const HospitalData* data);
    static std::string generateAppointmentReport(const HospitalData* data);
    static std::string generateStatisticsReport(const HospitalData* data);
    static std::string generateDotGraph(const HospitalData* data);
    static std::string generateTokenStatsReport(const std::map<TokenType, int>& freq, double timeMs);
    static std::string generateErrorReportHTML(const class ErrorManager& em);
};

#endif