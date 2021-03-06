#include "Element.hpp"

using namespace std;

using namespace Job;
using namespace SSDK;
using namespace SSDK::DB;

Element::Element()
{
    try
    {
        int role = Qt::UserRole;

        m_roleNames.insert(role++, "centralX");
        m_roleNames.insert(role++, "centralY");
        m_roleNames.insert(role++, "cwidth");
        m_roleNames.insert(role++, "cheight");
        m_roleNames.insert(role++, "shape");
    }
    CATCH_AND_RETHROW_EXCEPTION_WITH_OBJ("构造函数出错")
}

Element::~Element()
{
    try
    {
        for(int i = 0; i < this->m_pShapes.size(); ++i)
        {
            if(nullptr != this->m_pShapes[i])
            {
                delete this->m_pShapes[i];
            }
        }
    }
    catch(...)
    {
        for(int i = 0; i < this->m_pShapes.size(); ++i)
        {
            if(nullptr != this->m_pShapes[i])
            {
                delete this->m_pShapes[i];
            }
        }
    }
}

void Element::reset()
{
    try
    {
        for(int i = 0; i < this->m_pShapes.size(); ++i)
        {
            delete this->m_pShapes[i];
        }
    }
    CATCH_AND_RETHROW_EXCEPTION_WITH_OBJ("重置数据出错")
}

void Element::read()
{
    string pathPrefix = "file:/";                   //qml中获取的文件前缀
    string path = this->jobPath().toStdString();    //程式文件路径
    string::size_type pos = path.find( 'file:/' );  //从传入的路径中找出文件前缀
    path.erase( pos - pathPrefix.size() + 1, pathPrefix.size() );//删除文件前缀

    SqliteDB sqlite( path );
    try
    {
        auto isOpened = sqlite.isOpened();
        if( isOpened )
        {
            // 读取MeasuredObjs表
            string selectedString = "select * from MeasuredObjs";
            sqlite.prepare( selectedString );
            sqlite.begin();

            Shape *pShape = nullptr;
            Shape::ShapeType shapeType;
            string shape = "";
            while(true)
            {
                sqlite.step();
                if(sqlite.latestErrorCode() == SQLITE_DONE)
                {
                    break;
                }
                shape = boost::get<string>(sqlite.columnValue(0));
                transform(shape.begin(),shape.end(),shape.begin(),::toupper);
                if ( VAR_TO_STR(Shape::ShapeType::RECTANGLE) == shape )
                {
                    shapeType = Shape::ShapeType::RECTANGLE;
                }
                else if ( VAR_TO_STR(Shape::ShapeType::CIRCLE) == shape )
                {
                    shapeType = Shape::ShapeType::CIRCLE;
                }
                else
                {
                    THROW_EXCEPTION("读取被测对象形状失败！");
                }

                pShape = new Shape( shapeType,
                                    boost::get<int>(sqlite.columnValue(1)),
                                    boost::get<int>(sqlite.columnValue(2)),
                                    boost::get<int>(sqlite.columnValue(3)),
                                    boost::get<int>(sqlite.columnValue(4)) );

                this->pShapes().push_back(pShape);
            }

            sqlite.reset();
            sqlite.close();
        }
        else
        {
            THROW_EXCEPTION("程式加载失败！");
        }
    }
    catch( const CustomException& ex )
    {
        if( sqlite.isOpened() )
        {
            sqlite.reset();
            sqlite.close();
        }
        THROW_EXCEPTION( ex.what() );
    }
}

void Element::add(Shape::ShapeType shapeType, int centralX, int centralY, int width, int height)
{
    Shape *pShape = nullptr;
    pShape = new Shape(shapeType, centralX, centralY, width, height);
    this->pShapes().push_back(pShape);
}

void Element::save()
{
    SqliteDB sqlite;
    try
    {
        string pathPrefix = "file:/";                   //qml中获取的文件前缀
        string path = this->jobPath().toStdString();    //程式文件路径
        string::size_type pos = path.find( 'file:/' );  //从传入的路径中找出文件前缀
        path.erase( pos - pathPrefix.size() + 1, pathPrefix.size() );//删除文件前缀
        path.erase( path.size()-5,path.size() );         //删除后缀重新保存

        // 获取当前时间,用于生成保存的数据库文件名
        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("_hhmmss.'s'ung");

        // 创建数据库对象，打开传入路径的数据库
        sqlite.open( path + localTime.toStdString());

        if( !sqlite.isOpened() )
        {
            THROW_EXCEPTION("数据库打开失败！");
        }
        string sqlDrop = "DROP TABLE MeasuredObjs;";
        sqlite.execute( sqlDrop );

        string sqlCreate = "CREATE TABLE MeasuredObjs( "
                           "Shape TEXT, "
                           "CentralX INTEGER, "
                           "CentralY INTEGER, "
                           "Width INTEGER, "
                           "Height INTEGER); ";
        sqlite.execute( sqlCreate );

        string sqlInsert = "INSERT INTO MeasuredObjs( "
                           "Shape, CentralX, CentralY, Width, Height) "
                           "VALUES(?,?,?,?,?);";

        sqlite.prepare( sqlInsert );
        sqlite.begin();
        string shapeType = "";

        for (QVector<Shape *>::iterator iter = this->pShapes().begin();
             iter != this->pShapes().end(); ++iter)
        {
            // 判断生成的被测对象类型，然后设置相应的类型
            if (Shape::ShapeType::RECTANGLE == (*iter)->shapeType() )
            {
                shapeType = VAR_TO_STR(Shape::ShapeType::RECTANGLE);
            }
            else if (Shape::ShapeType::CIRCLE == (*iter)->shapeType() )
            {
                shapeType = VAR_TO_STR(Shape::ShapeType::CIRCLE);
            }
            else
            {
                THROW_EXCEPTION("被测对象形状错误！");
            }
            transform(shapeType.begin(),shapeType.end(),shapeType.begin(),::tolower);

            sqlite.execute( sqlInsert, shapeType,
                                       (*iter)->centralX(),
                                       (*iter)->centralY(),
                                       (*iter)->width(),
                                       (*iter)->height() );
        }
        sqlite.commit();

        sqlite.close();
    }
    catch( const CustomException& ex )
    {
        if( sqlite.isOpened() )
        {
            sqlite.reset();
            sqlite.close();
        }
        THROW_EXCEPTION( ex.what() );
    }
}


