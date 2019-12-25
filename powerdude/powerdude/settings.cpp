#include "settings.h"
#include "application.h"

Settings::Settings()
	:QSettings(qApp->applicationDirPath() + "\\settings.ini", QSettings::IniFormat)
{
	this->setIniCodec(QTextCodec::codecForName("UTF-8"));
}


Settings::~Settings()
{
}

void Settings::setOneValue(const QString & group, const QString & key, const QVariant & value)
{
	QSettings::beginGroup(group);
	QSettings::setValue(key, value);
	QSettings::endGroup();
}

QVariant Settings::oneValue(const QString & group, const QString & key, const QVariant & defaultValue)
{
	QSettings::beginGroup(group);
	QVariant v = QSettings::value(key, defaultValue);
	QSettings::endGroup();
	return v;
}

//дxml
void Settings::WriteXml()
{
	//�򿪻򴴽��ļ�
	QFile file("test.xml"); //���·��������·������Դ·��������
	if (!file.open(QFile::WriteOnly | QFile::Truncate)) //������QIODevice��Truncate��ʾ���ԭ��������
		return;

	QDomDocument doc;
	//д��xmlͷ��
	QDomProcessingInstruction instruction; //��Ӵ�������
	instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instruction);
	//��Ӹ��ڵ�
	QDomElement root = doc.createElement("library");
	doc.appendChild(root);
	//��ӵ�һ���ӽڵ㼰����Ԫ��
	QDomElement book = doc.createElement("book");
	book.setAttribute("id", 1); //��ʽһ����������  ���м�ֵ�Ե�ֵ�����Ǹ�������
	QDomAttr time = doc.createAttribute("time"); //��ʽ������������ ֵ�������ַ���
	time.setValue("2013/6/13");
	book.setAttributeNode(time);
	QDomElement title = doc.createElement("title"); //������Ԫ��
	QDomText text; //�������ű�ǩ�м��ֵ
	text = doc.createTextNode("C++ primer");
	book.appendChild(title);
	title.appendChild(text);
	QDomElement author = doc.createElement("author"); //������Ԫ��
	text = doc.createTextNode("Stanley Lippman");
	author.appendChild(text);
	book.appendChild(author);
	root.appendChild(book);

	//��ӵڶ����ӽڵ㼰����Ԫ�أ����ֱ���ֻ�����¸�ֵ
	book = doc.createElement("book");
	book.setAttribute("id", 2);
	time = doc.createAttribute("time");
	time.setValue("2007/5/25");
	book.setAttributeNode(time);
	title = doc.createElement("title");
	text = doc.createTextNode("Thinking in Java");
	book.appendChild(title);
	title.appendChild(text);
	author = doc.createElement("author");
	text = doc.createTextNode("Bruce Eckel");
	author.appendChild(text);
	book.appendChild(author);
	root.appendChild(book);

	//������ļ�
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //����4��
	file.close();

}

//��xml
void Settings::ReadXml()
{
	//�򿪻򴴽��ļ�
	QFile file("test.xml"); //���·��������·������Դ·������
	if (!file.open(QFile::ReadOnly))
		return;

	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement(); //���ظ��ڵ�
	qDebug() << root.nodeName();
	QDomNode node = root.firstChild(); //��õ�һ���ӽڵ�
	while (!node.isNull())  //����ڵ㲻��
	{
		if (node.isElement()) //����ڵ���Ԫ��
		{
			QDomElement e = node.toElement(); //ת��ΪԪ�أ�ע��Ԫ�غͽڵ����������ݽṹ����ʵ���
			qDebug() << e.tagName() << " " << e.attribute("id") << " " << e.attribute("time"); //��ӡ��ֵ�ԣ�tagName��nodeName��һ������
			QDomNodeList list = e.childNodes();
			for (int i = 0;i < list.count();i++) //������Ԫ�أ�count��size��������,�����ڱ�ǩ������
			{
				QDomNode n = list.at(i);
				if (node.isElement())
					qDebug() << n.nodeName() << ":" << n.toElement().text();
			}
		}
		node = node.nextSibling(); //��һ���ֵܽڵ�,nextSiblingElement()����һ���ֵ�Ԫ�أ������
	}

}

//����xml����
void Settings::AddXml()
{
	//���ļ�
	QFile file("test.xml"); //���·��������·������Դ·��������
	if (!file.open(QFile::ReadOnly))
		return;

	//����һ��һ���ӽڵ��Լ�Ԫ��
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement();
	QDomElement book = doc.createElement("book");
	book.setAttribute("id", 3);
	book.setAttribute("time", "1813/1/27");
	QDomElement title = doc.createElement("title");
	QDomText text;
	text = doc.createTextNode("Pride and Prejudice");
	title.appendChild(text);
	book.appendChild(title);
	QDomElement author = doc.createElement("author");
	text = doc.createTextNode("Jane Austen");
	author.appendChild(text);
	book.appendChild(author);
	root.appendChild(book);

	if (!file.open(QFile::WriteOnly | QFile::Truncate)) //�ȶ�����������д���������truncate�����ں���׷�����ݣ�����Ч��
		return;
	//������ļ�
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //����4��
	file.close();
}

//ɾ��xml����
void RemoveXml()
{
	//���ļ�
	QFile file("test.xml"); //���·��������·������Դ·��������
	if (!file.open(QFile::ReadOnly))
		return;

	//ɾ��һ��һ���ӽڵ㼰��Ԫ�أ����ڵ�ɾ���ڲ�ڵ��ڴ���ͬ
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();  //һ��Ҫ�ǵùص�������Ȼ�޷���ɲ���

	QDomElement root = doc.documentElement();
	QDomNodeList list = doc.elementsByTagName("book"); //�ɱ�ǩ����λ
	for (int i = 0;i < list.count();i++)
	{
		QDomElement e = list.at(i).toElement();
		if (e.attribute("time") == "2007/5/25")  //����������λ��������hash�ķ�ʽ��warning���������ɾ��һ���ڵ㣬��ʵ���ԼӸ�break
			root.removeChild(list.at(i));
	}

	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return;
	//������ļ�
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //����4��
	file.close();
}

//����xml����
void UpdateXml()
{
	//���ļ�
	QFile file("test.xml"); //���·��������·������Դ·��������
	if (!file.open(QFile::ReadOnly))
		return;

	//����һ����ǩ��,���֪��xml�Ľṹ��ֱ�Ӷ�λ���Ǹ���ǩ�϶������
	//�����ñ����ķ���ȥƥ��tagname����attribut��value������
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement();
	QDomNodeList list = root.elementsByTagName("book");
	QDomNode node = list.at(list.size() - 1).firstChild(); //��λ��������һ���ӽڵ����Ԫ��
	QDomNode oldnode = node.firstChild(); //��ǩ֮���������Ϊ�ڵ���ӽڵ����,��ǰ��Pride and Projudice
	node.firstChild().setNodeValue("Emma");
	QDomNode newnode = node.firstChild();
	node.replaceChild(newnode, oldnode);

	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return;
	//������ļ�
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //����4��
	file.close();
}

