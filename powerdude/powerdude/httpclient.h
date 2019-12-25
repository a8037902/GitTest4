#pragma once


#include <functional>

class QString;
class QByteArray;
struct HttpClientPrivate;
class QNetworkReply;
class QNetworkAccessManager;
class QGlobal;

class HttpClient {
public:
    HttpClient(const QString &url);
    ~HttpClient();

    /**
    * @brief ÿ����һ�� QNetworkAccessManager ���󶼻ᴴ��һ���̣߳���Ƶ���ķ�������ʱ��Ϊ�˽�ʡ�߳���Դ��
    * ����ʹ�ô��˵� QNetworkAccessManager�������ᱻ HttpClient ɾ����
    * ���û��ʹ�� useManager() ����һ�� QNetworkAccessManager���� HttpClient ���Զ��Ĵ���һ�������������������ɺ�ɾ������
    * @param manager QNetworkAccessManager ����
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& useManager(QNetworkAccessManager *manager);

    /**
    * @brief ���� debug Ϊ true ��ʹ�� debug ģʽ������ִ��ʱ�������� URL �Ͳ�����
    * @param debug �Ƿ����õ���ģʽ
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& debug(bool debug);

    /**
    * @brief ���Ӳ���
    * @param name ����������
    * @param value ������ֵ
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& addParam(const QString &name, const QString &value);

    /**
    * @brief ���Ӳ���
    * @param jsonString Json��ʽ������� 
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& addParam(const QString &jsonString);

    /**
    * @brief ���ӷ���ͷ
    * @param header ����ͷ������
    * @param value ����ͷ��ֵ
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& addHeader(const QString &header, const QString &value);

    /**
    * @brief ��� POST ��ʹ�õ�ͷ��Ϣ���ȼ��� addHeader("content-type", "application/x-www-form-urlencoded")
    * @return ���� HttpClient �����ã�����������ʽ����
    */
    HttpClient& addFormHeader();

    /**
    * @brief ִ�� GET ����
    * @param successHandler ����ɹ��Ļص� lambda ����
    * @param errorHandler ����ʧ�ܵĻص� lambda ����
    * @param encoding ������Ӧ�ı���
    */
    void get(std::function<void(const QString &)> successHandler,
        std::function<void(const QString &)> errorHandler = NULL,
        const char *encoding = "UTF-8");

    /**
    * @brief ִ�� POST ����
    * @param successHandler ����ɹ��Ļص� lambda ����
    * @param errorHandler ����ʧ�ܵĻص� lambda ����
    * @param encoding ������Ӧ�ı���
    */
    void post(std::function<void(const QString &)> successHandler,
        std::function<void(const QString &)> errorHandler = NULL,
        const char *encoding = "UTF-8");

    /**
    * @brief ʹ�� GET �������أ��������ݿɶ�ȡʱ�ص� readyRead(), ����������Ӧ���� readyRead() ������ݱ��浽�ļ�
    * @param readyRead �����ݿɶ�ȡʱ�Ļص� lambda ����
    * @param finishHandler ��������ɺ�Ļص� lambda ����
    * @param errorHandler ����ʧ�ܵĻص� lambda ����
    * @param downloadProgressHandler �������ؽ��ȵĻص� lambda ����
    */
    void download(std::function<void(const QByteArray &)> readyRead,
        std::function<void(const QByteArray &)> finishHandler = NULL,
        std::function<void(const QString &)> errorHandler = NULL,
        std::function<void(__int64 & bytesReceived, __int64 & bytesTotal)> downloadProgressHandler = NULL);

    /**
    * @brief �ϴ��ļ�
    * @param path Ҫ�ϴ����ļ���·��
    * @param successHandler ����ɹ��Ļص� lambda ����
    * @param errorHandler ����ʧ�ܵĻص� lambda ����
    * @param encoding ������Ӧ�ı���
    */
    void upload(const QString &path, std::function<void(const QString &)> successHandler = NULL,
        std::function<void(const QString &)> errorHandler = NULL,
        const char *encoding = "UTF-8");

private:
    /**
    * @brief ִ������ĸ�������
    * @param posted Ϊ true ��ʾ POST ����Ϊ false ��ʾ GET ����
    * @param successHandler ����ɹ��Ļص� lambda ����
    * @param errorHandler ����ʧ�ܵĻص� lambda ����
    * @param encoding ������Ӧ�ı���
    */
    void execute(bool posted,
        std::function<void(const QString &)> successHandler,
        std::function<void(const QString &)> errorHandler,
        const char *encoding);

    /**
    * @brief ��ȡ��������Ӧ������
    * @param reply ����� QNetworkReply ����
    * @param encoding ������Ӧ�ı��룬Ĭ��ʹ�� UTF-8
    * @return ����������Ӧ���ַ���
    */
    QString readResponse(QNetworkReply *reply, const char *encoding = "UTF-8");

    HttpClientPrivate *d;
};


