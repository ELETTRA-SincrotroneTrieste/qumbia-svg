#ifndef QUSVGJSON_H
#define QUSVGJSON_H


class QuSvgJson
{
public:
    QuSvgJson();
    bool readJsonFile(const QString &file_path, QJsonDocument &json_doc);
};

#endif // QUSVGJSON_H
