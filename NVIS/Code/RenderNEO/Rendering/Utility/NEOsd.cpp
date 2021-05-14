#include "NEOsd.h"
#include "Utility/NEOsdMsg.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Manager/NESimulationManager.h"

#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Rendering/Manager/NEIGLSurface.h"
#include "NENode.h"

NEOsd& NEOsd::Instance()
{
    static NEOsd instance;
    return instance;
}

/*!
*/

NEOsd::NEOsd()
{
    QObject::connect( &NEBASE, SIGNAL(initOSD()), this, SLOT(init()), Qt::DirectConnection );
}

void NEOsd::add(uint rank, const NEOsdMsg& message, const NEObject* object)
{
    QMap<uint, NEOsdMsg>* pPositionMap = getPositionMap(object->OsdMsgPos());

    if(pPositionMap)
    {
        (*pPositionMap)[object->osdStartRank()+rank] = message;
        (*pPositionMap)[object->osdStartRank()] = NEOsdMsg(object->objectName() + ":");
    }
    else
        Q_ASSERT_X(false, "NEOsd::add", "No such OSD exists");
}

void NEOsd::add(uint rank, const NEOsdMsg& message)
{
    QMap<uint, NEOsdMsg>* pPositionMap = getPositionMap(message.pos);
    if(pPositionMap)
        (*pPositionMap)[rank] = message;
    else
        Q_ASSERT_X(false, "NEOsd::add", "No such OSD exists");

}

bool NEOsd::logging()
{
    return m_Logging;

}

void NEOsd::setLogging(bool flag)
{
    m_Logging = flag;
}

uint NEOsd::logsPerSecondTop() const
{
    return m_LogsPerSecondTop;
}

void NEOsd::setLogsPerSecondTop(uint val)
{
    m_LogsPerSecondTop = val;
}

uint NEOsd::logsPerSecondTopRight() const
{
    return m_LogsPerSecondTopRight;
}

void NEOsd::setLogsPerSecondTopRight(uint val)
{
    m_LogsPerSecondTopRight = val;
}

uint NEOsd::logsPerSecondRight() const
{
    return m_LogsPerSecondRight;
}

void NEOsd::setLogsPerSecondRight(uint val)
{
    m_LogsPerSecondRight = val;
}

uint NEOsd::logsPerSecondBottomRight() const
{
    return m_LogsPerSecondBottomRight;
}

void NEOsd::setLogsPerSecondBottomRight(uint val)
{
    m_LogsPerSecondBottomRight = val;
}

uint NEOsd::logsPerSecondBottom() const
{
    return m_LogsPerSecondBottom;
}

void NEOsd::setLogsPerSecondBottom(uint val)
{
    m_LogsPerSecondBottom = val;
}

uint NEOsd::logsPerSecondBottomLeft() const
{
    return m_LogsPerSecondBottomLeft;
}

void NEOsd::setLogsPerSecondBottomLeft(uint val)
{
    m_LogsPerSecondBottomLeft = val;
}

uint NEOsd::logsPerSecondLeft() const
{
    return m_LogsPerSecondLeft;
}

void NEOsd::setLogsPerSecondLeft(uint val)
{
    m_LogsPerSecondLeft = val;
}

uint NEOsd::logsPerSecondTopLeft() const
{
    return m_LogsPerSecondTopLeft;
}

void NEOsd::setLogsPerSecondTopLeft(uint val)
{
    m_LogsPerSecondTopLeft = val;
}

bool NEOsd::loggingTop() const
{
    return m_LoggingTop;
}

void NEOsd::setLoggingTop(bool val)
{
    m_LoggingTop = val;
}

bool NEOsd::loggingTopRight() const
{
    return m_LoggingTopRight;
}

void NEOsd::setLoggingTopRight(bool val)
{
    m_LoggingTopRight = val;
}

bool NEOsd::loggingRight() const
{
    return m_LoggingRight;
}

void NEOsd::setLoggingRight(bool val)
{
    m_LoggingRight = val;
}

bool NEOsd::loggingBottomRight() const
{
    return m_LoggingBottomRight;
}

void NEOsd::setLoggingBottomRight(bool val)
{
    m_LoggingBottomRight = val;
}

bool NEOsd::loggingBottom() const
{
    return m_LoggingBottom;
}

void NEOsd::setLoggingBottom(bool val)
{
    m_LoggingBottom = val;
}

bool NEOsd::loggingBottomLeft() const
{
    return m_LoggingBottomLeft;
}

void NEOsd::setLoggingBottomLeft(bool val)
{
    m_LoggingBottomLeft = val;
}

bool NEOsd::loggingLeft() const
{
    return m_LoggingLeft;
}

void NEOsd::setLoggingLeft(bool val)
{
    m_LoggingLeft = val;
}

bool NEOsd::loggingTopLeft() const
{
    return m_LoggingTopLeft;
}

void NEOsd::setLoggingTopLeft(bool val)
{
    m_LoggingTopLeft = val;
}

bool NEOsd::requiresLog(uint lps, const double elapsedTime)
{
    if(lps == 0) return true;
    else return (elapsedTime > (1.f / lps) * m_LogsRecorded / (100.f / lps));
}

bool NEOsd::requiresLog(uint lps)
{
    if(m_LogsRecorded && lps > 0) return ((int)((m_LogsRecorded - 1) / (100.f / lps)) < (int)((m_LogsRecorded) / (100.f / lps)));
    else return true;
}

void NEOsd::managersCreated(QList<std::shared_ptr<NEManager> > managers)
{
    QList<std::shared_ptr<NEManager> >::const_iterator ssItr = managers.begin();
    for( ; ssItr != managers.end(); ++ssItr )
    {
        QObject::connect( ssItr->get(), SIGNAL(addOSD(uint, const NEOsdMsg&)),
                          this, SLOT(add(uint, const NEOsdMsg&)),
                          Qt::DirectConnection );
        QObject::connect( ssItr->get(), SIGNAL(removeOSD(uint, NEOsdMsgPos)),
                          this, SLOT(remove(uint, NEOsdMsgPos)),
                          Qt::DirectConnection );
    }
    return;
}

void NEOsd::managerCreated( NEManager* pManager )
{
    if(!pManager)
        return;
    QObject::connect( pManager,SIGNAL(addOSD(uint, const NEOsdMsg&)),
                      this, SLOT(add(uint, const NEOsdMsg&)),
                      Qt::DirectConnection );
    QObject::connect( pManager,SIGNAL(removeOSD(uint, NEOsdMsgPos)),
                      this, SLOT(remove(uint, NEOsdMsgPos)),
                      Qt::DirectConnection );
    return;
}

void NEOsd::objectCreated(NENode *object)
{
    QObject::connect( object,SIGNAL(addOSD(uint, const NEOsdMsg&)),
                      this, SLOT(add(uint, const NEOsdMsg&)),
                      Qt::DirectConnection );
    QObject::connect( object,SIGNAL(addOSD(uint, const NEOsdMsg&, const NEObject *)),
                      this, SLOT(add(uint, const NEOsdMsg&, const NEObject *)),
                      Qt::DirectConnection );
    QObject::connect( object,SIGNAL(removeOSD(uint, NEOsdMsgPos)),
                      this, SLOT(remove(uint, NEOsdMsgPos)),
                      Qt::DirectConnection );
    QObject::connect( object,SIGNAL(removeOSD(uint, const NEObject *)),
                      this, SLOT(remove(uint, const NEObject *)),
                      Qt::DirectConnection );
    QObject::connect( object,
                      SIGNAL(moveBufferOSD( const NEObject::NEOsdMsgPos oldPos, const NEObject::NEOsdMsgPos newPos, const NEObject* object )),
                      this, SLOT(moveBuffer( const NEObject::NEOsdMsgPos oldPos, const NEObject::NEOsdMsgPos newPos, const NEObject* object )),
                      Qt::DirectConnection );

}

void NEOsd::init()
{
    m_LoggingTop         = false;
    m_LoggingTopRight    = false;
    m_LoggingRight       = false;
    m_LoggingBottomRight = false;
    m_LoggingBottom      = false;
    m_LoggingBottomLeft  = false;
    m_LoggingLeft        = false;
    m_LoggingTopLeft     = false;

    m_LogsPerSecondTop          = 10;
    m_LogsPerSecondTopRight     = 10;
    m_LogsPerSecondRight        = 10;
    m_LogsPerSecondBottomRight  = 10;
    m_LogsPerSecondBottom       = 10;
    m_LogsPerSecondBottomLeft   = 10;
    m_LogsPerSecondLeft         = 10;
    m_LogsPerSecondTopLeft      = 10;

    m_LogFolderName             = "";
    m_LogsRecorded              = 0;
    m_LogsTopRecorded           = 0;
    m_LogsRightRecorded         = 0;

    setParent( &NEBASE );
    setObjectName( "Logger" );

    if( NEBASE.guiMode() )
    {
        QObject::connect( &NESCENE, SIGNAL(clearBuffers()), this, SLOT(clearBuffers()), Qt::DirectConnection );

        QObject::connect( &NESCENE, SIGNAL(addOSD(uint, const NEOsdMsg&)),
                          this, SLOT(add(uint, const NEOsdMsg&)),
                          Qt::DirectConnection );
        QObject::connect( &NESIMMGR, SIGNAL(addOSD(uint, const NEOsdMsg&)),
                          this, SLOT(add(uint, const NEOsdMsg&)),
                          Qt::DirectConnection );
        QObject::connect( &NESCENE, SIGNAL(removeOSD(uint, NEOsdMsgPos)),
                          this, SLOT(remove(uint, NEOsdMsgPos)),
                          Qt::DirectConnection );
        QObject::connect( &NESIMMGR, SIGNAL(removeOSD(uint, NEOsdMsgPos)),
                          this, SLOT(remove(uint, NEOsdMsgPos)),
                          Qt::DirectConnection );
        QObject::connect( &NESCENE, SIGNAL(logToFileOSD(QString, const double)),
                          this, SLOT(logToFile(QString, const double)),
                          Qt::DirectConnection );

        QObject::connect( &NESCENE, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                          this, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)),
                          Qt::DirectConnection );
        QObject::connect( &NESIMMGR, SIGNAL(managersCreated(QList<std::shared_ptr<NEManager> >) ),
                          this, SLOT(managersCreated(QList<std::shared_ptr<NEManager> >)),
                          Qt::DirectConnection );

        QObject::connect( &NESIMMGR, SIGNAL(managerCreated(NEManager *) ),
                          this, SLOT(managerCreated(NEManager *)),
                          Qt::DirectConnection );

        QObject::connect( &NESCENE, SIGNAL(objectCreated(NENode*)), this, SLOT(objectCreated(NENode*)), Qt::DirectConnection );
    }
}

void NEOsd::clearBuffers()
{
    messagesTop.clear();
    messagesTopRight.clear();
    messagesRight.clear();
    messagesBottomRight.clear();
    messagesBottom.clear();
    messagesBottomLeft.clear();
    messagesLeft.clear();
    messagesTopLeft.clear();
    //setLogging(false);
}

void NEOsd::drawRect(IGLSurface* surface, int x1, int y1, int x2, int y2)
{
    float currentColor[4];
    GL->glGetFloatv( GL_CURRENT_COLOR, currentColor );
    GL->glUniform4fv( GL->glGetUniformLocation( surface->shaderProgram()->programId(), "meshColor" ), 1, currentColor );

    GLshort vtx1[] = { (GLshort)x1, (GLshort)y1, (GLshort)x1, (GLshort)y2, (GLshort)x2, (GLshort)y2, (GLshort)x2, (GLshort)y1};
    GL->glEnableClientState( GL_VERTEX_ARRAY );
    GL->glVertexPointer( 2, GL_SHORT, 0, vtx1 );
    GL->glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    GL->glDisableClientState( GL_VERTEX_ARRAY);
}

void NEOsd::draw(IGLSurface* surface, int width, int height)
{
    GL->glPushAttrib(GL_ALL_ATTRIB_BITS);

    surface->setShaderProgram(0);
    NERENDERMAN.GLManager()->setBlendFunc( kOpacityBlendFunc );


    int xRes = width;
    int yRes = height;

    float fontWidth = offsety / 1.8;


    int rightMiddleHeight = yRes - (yRes * 0.5) + messagesRight.size() * 0.5 * offsety;
    int leftMiddleHeight = yRes - (yRes * 0.5) + messagesLeft.size() * 0.5 * offsety;



    uint leftMargin = 5, topMargin = offsety;

    uint posyU = topMargin, widthU = 0;
    uint posyUR = topMargin, widthUR = 0;
    uint posyR = rightMiddleHeight, widthR = 0;
    uint posyBR = 5, widthBR = 0;
    uint posyB = 5, widthB = 0;
    uint posyBL = 5, widthBL = 0;
    uint posyL = leftMiddleHeight, widthL = 0;
    uint posyUL = topMargin, widthUL = 0;

    bool usedU = false;
    bool usedUR = false;
    bool usedR = false;
    bool usedBR = false;
    bool usedB = false;
    bool usedBL = false;
    bool usedL = false;
    bool usedUL = false;


    QString text;


    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesTop.begin();
        msgIter != messagesTop.end(); ++ msgIter)
    {
        usedU = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthU = MAX(widthU, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), (xRes >> 1) - (widthU >> 1), yRes - posyU);
        posyU += offsety;
    }

    QMapIterator<u_int, NEOsdMsg> msgIter(messagesTopRight);
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedUR = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthUR = MAX(widthUR, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), xRes - texLen, yRes - posyUR);
        posyUR += offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesRight.begin();
        msgIter != messagesRight.end(); ++ msgIter)
    {
        usedR = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthR = MAX(widthR, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), xRes - texLen, posyR);
        posyR -= offsety;
    }

    msgIter = messagesBottomRight;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedBR = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthBR = MAX(widthBR, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), xRes - texLen, posyBR);
        posyBR += offsety;
    }

    msgIter = messagesBottom;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedB = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthB = MAX(widthB, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), (xRes >> 1) - (widthB >> 1), posyB);
        posyB += offsety;
    }

    msgIter = messagesBottomLeft;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedBL = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthBL = MAX(widthBL, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), leftMargin, posyBL);
        posyBL += offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesLeft.begin();
        msgIter != messagesLeft.end(); ++ msgIter)
    {
        usedL = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthL = MAX(widthL, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), leftMargin, posyL);
        posyL -= offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesTopLeft.begin();
        msgIter != messagesTopLeft.end(); ++ msgIter)
    {
        usedUL = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth + fontWidth;
        widthUL = MAX(widthUL, texLen);
        NERENDERMAN.GLManager()->glPrint(((*m_ItemName) + (data.m_ItemText)).toUtf8(), leftMargin, yRes - posyUL);
        posyUL += offsety;
    }

    NERENDERMAN.GLManager()->setDepthTest(kLessDepthTest);
    //glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    NERENDERMAN.GLManager()->bindPositionShader(surface);
    NERENDERMAN.GLManager()->updateCPUniforms(surface);


    static Color32 OsdRed(255, 76, 76, 40);
    static Color32 OsdBlue(0, 153, 255, 40);
    static Color32 OsdLine(30, 183, 255, 170);

    if(usedU)
    {
        if(m_LoggingTop && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, (xRes >> 1) - (widthU >> 1) - leftMargin, yRes, (xRes >> 1) - (widthU >> 1) + widthU, yRes - posyU + leftMargin + 1);
    }
    if(usedUR)
    {
        if(m_LoggingTopRight && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, xRes - widthUR - leftMargin, yRes - posyUR + leftMargin + 1, xRes, yRes);
    }
    if(usedR)
    {
        if(m_LoggingRight && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, xRes - widthR - leftMargin, posyR + leftMargin, xRes, rightMiddleHeight + offsety + leftMargin);
    }
    if(usedBR)
    {
        if(m_LoggingBottomRight && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, xRes - widthBR - leftMargin, 0, xRes, posyBR);
    }
    if(usedB)
    {
        if(m_LoggingBottom && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, (xRes >> 1) - (widthB >> 1) - leftMargin, 0, (xRes >> 1) - (widthB >> 1) + widthB, posyB);
    }
    if(usedBL)
    {
        if(m_LoggingBottomLeft && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, 0, 0, widthBL, posyBL);
    }
    if(usedL)
    {
        if(m_LoggingLeft && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, 0, posyL + leftMargin, widthL, leftMiddleHeight + offsety);
    }
    if(usedUL)
    {
        if(m_LoggingTopLeft && m_Logging) GL->glColor4ubv((GLubyte*)&OsdRed);
        else GL->glColor4ubv((GLubyte*)&OsdBlue);
        drawRect(surface, 0, yRes, widthUL, yRes - posyUL + leftMargin + 1);
    }

    NERENDERMAN.GLManager()->setFillMode(kWireframeFrontAndBack);
    GL->glColor4ubv((GLubyte*)&OsdLine);


    if(usedU)
        drawRect(surface, (xRes >> 1) - (widthU >> 1) - leftMargin, yRes, (xRes >> 1) - (widthU >> 1) + widthU, yRes - posyU + leftMargin + 1);
    if(usedUR)
        drawRect(surface, xRes - widthUR - leftMargin, yRes - posyUR + leftMargin + 1, xRes, yRes);
    if(usedR)
        drawRect(surface, xRes - widthR - leftMargin, posyR + leftMargin, xRes, rightMiddleHeight + offsety + leftMargin);
    if(usedBR)
        drawRect(surface, xRes - widthBR - leftMargin, 0, xRes, posyBR);
    if(usedB)
        drawRect(surface, (xRes >> 1) - (widthB >> 1) - leftMargin, 0, (xRes >> 1) - (widthB >> 1) + widthB, posyB);
    if(usedBL)
        drawRect(surface, 0, 0, widthBL, posyBL);
    if(usedL)
        drawRect(surface, 0, posyL + leftMargin, widthL, leftMiddleHeight + offsety);
    if(usedUL)
        drawRect(surface, 0, yRes, widthUL, yRes - posyUL + leftMargin + 1);

    //	glEndPrint();

    NERENDERMAN.GLManager()->setDepthTest(kDisableDepthTest);

    GL->glPopAttrib();
}

void NEOsd::drawInQt(int width, int height, QPainter* painter)
{
    m_MsgPos.clear();

    int xRes = width;
    int yRes = height;
    //painter->setFont(*NEGUISTYLE.MonoNormalFont());
    static float fontWidth = 1 + painter->fontInfo().pixelSize() * 0.5;

    static u_int offsety = 12;

    int rightMiddleHeight = yRes - (yRes * 0.5) + messagesRight.size() * 0.5 * offsety;
    int leftMiddleHeight = yRes - (yRes * 0.5) + messagesLeft.size() * 0.5 * offsety;

    static u_int leftMargin = 5, topMargin = 12;

    u_int posyU = topMargin, widthU = 0;
    u_int posyUR = topMargin, widthUR = 0;
    u_int posyR = rightMiddleHeight, widthR = 0;
    u_int posyBR = 5, widthBR = 0;
    u_int posyB = 5, widthB = 0;
    u_int posyBL = 5, widthBL = 0;
    u_int posyL = leftMiddleHeight, widthL = 0;
    u_int posyUL = topMargin, widthUL = 0;

    bool usedU = false;
    bool usedUR = false;
    bool usedR = false;
    bool usedBR = false;
    bool usedB = false;
    bool usedBL = false;
    bool usedL = false;
    bool usedUL = false;

    QPen p = QPen();
    p.setColor(Qt::white);
    p.setWidth(1);
    painter->setPen(p);
    QString text;


    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesTop.begin();
        msgIter != messagesTop.end(); ++ msgIter)
    {
        usedU = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;
        widthU = MAX(widthU, texLen);

        m_MsgPos.push_back(MessageAndPos(text, (xRes >> 1) - (widthU >> 1), posyU));

        posyU += offsety;
    }

    QMapIterator<u_int, NEOsdMsg> msgIter(messagesTopRight);
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedUR = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;

        u_int texLen = (float)text.length() * fontWidth;

        if(data.m_ItemText.length() < 2)
            texLen += fontWidth;
        widthUR = MAX(widthUR, texLen);

        m_MsgPos.push_back(MessageAndPos(text, xRes - texLen, posyUR));

        posyUR += offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesRight.begin();
        msgIter != messagesRight.end(); ++ msgIter)
    {
        usedR = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;

        if(data.m_ItemText.length() < 2)
            texLen += fontWidth;

        widthR = MAX(widthR, texLen);

        m_MsgPos.push_back(MessageAndPos(text, xRes - texLen, posyR));

        posyR -= offsety;
    }

    msgIter = messagesBottomRight;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedBR = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;

        if(data.m_ItemText.length() < 2)
            texLen += fontWidth;
        widthBR = MAX(widthBR, texLen);

        m_MsgPos.push_back(MessageAndPos(text, xRes - texLen, yRes - posyBR));

        posyBR += offsety;
    }

    msgIter = messagesBottom;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedB = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;
        widthB = MAX(widthB, texLen);

        m_MsgPos.push_back(MessageAndPos(text, (xRes >> 1) - (widthB >> 1), yRes - posyB));

        posyB += offsety;
    }

    msgIter = messagesBottomLeft;
    msgIter.toBack();
    while(msgIter.hasPrevious())
    {
        usedBL = true;
        msgIter.previous();
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        const NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;
        widthBL = MAX(widthBL, texLen);

        m_MsgPos.push_back(MessageAndPos(text, leftMargin, yRes - posyBL));

        posyBL += offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesLeft.begin();
        msgIter != messagesLeft.end(); ++ msgIter)
    {
        usedL = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;
        widthL = MAX(widthL, texLen);

        m_MsgPos.push_back(MessageAndPos(text, leftMargin, posyL));

        posyL -= offsety;
    }

    for(QMap<u_int, NEOsdMsg>::iterator msgIter = messagesTopLeft.begin();
        msgIter != messagesTopLeft.end(); ++ msgIter)
    {
        usedUL = true;
        const QString* m_ItemName = &msgIter.value().m_ItemName;
        NEOsdMsg& data = msgIter.value();
        text = *m_ItemName + data.m_ItemText;
        u_int texLen = (float)text.length() * fontWidth;
        widthUL = MAX(widthUL, texLen);

        m_MsgPos.push_back(MessageAndPos(text, leftMargin, posyUL));

        posyUL += offsety;
    }


    static QColor OsdRed(255, 76, 76, 40);
    static QColor OsdBlue(0, 153, 255, 40);
    static QColor OsdLine(30, 183, 255, 170);

    if(usedU)
    {
        QRect rect(QPoint((xRes >> 1) - (widthU >> 1) - leftMargin, 0), QPoint((xRes >> 1) - (widthU >> 1) + widthU, posyU + leftMargin + 1 - offsety));
        if(m_LoggingTop && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedUR)
    {
        QRect rect(QPoint(xRes - widthUR - leftMargin, posyUR - leftMargin - 1), QPoint(xRes, 0));
        if(m_LoggingTopRight && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedR)
    {
        QRect rect(QPoint(xRes - widthR - leftMargin, posyR + leftMargin), QPoint(xRes, rightMiddleHeight + leftMargin));
        if(m_LoggingRight && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedBR)
    {
        QRect rect(QPoint(xRes - widthBR - leftMargin, yRes), QPoint(xRes, yRes - posyBR));
        if(m_LoggingBottomRight && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedB)
    {
        QRect rect(QPoint((xRes >> 1) - (widthB >> 1) - leftMargin, yRes), QPoint((xRes >> 1) - (widthB >> 1) + widthB, posyB));
        if(m_LoggingBottom && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedBL)
    {
        QRect rect(QPoint(0, yRes), QPoint(widthBL, yRes - posyBL));
        if(m_LoggingBottomLeft && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedL)
    {
        QRect rect(QPoint(0, posyL + leftMargin), QPoint(widthL, leftMiddleHeight));
        if(m_LoggingLeft && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }
    if(usedUL)
    {
        QRect rect(QPoint(0, 0), QPoint(widthUL, posyUL + leftMargin + 1 - offsety));
        if(m_LoggingTopLeft && m_Logging) painter->fillRect(rect, OsdRed);
        else painter->fillRect(rect, OsdBlue);
    }

    for(MessageAndPos m : m_MsgPos)
    {
        painter->drawText(m.posx, m.posy, m.msg);
    }
}

void NEOsd::remove(uint rank, NEObject::NEOsdMsgPos position)
{
    QMap<uint, NEOsdMsg>* pPositionMap = getPositionMap(position);
    if(pPositionMap)
        pPositionMap->remove(rank);
    else
        Q_ASSERT_X(false, "NEOsd::remove", "No such OSD exists");
}

void NEOsd::remove(uint rank, const NEObject* object)
{
    QMap<uint, NEOsdMsg>* pPositionMap = getPositionMap(object->OsdMsgPos());
    if(pPositionMap)
    {
        pPositionMap->remove(object->osdStartRank()+rank);
    }
    else
        Q_ASSERT_X(false, "NEOsd::remove", "No such OSD exists");
}

/*
        Logs the osd to a file in the scene path / log folder
*/
uint NEOsd::logToFile(QString scenePath, const double elapsedTime)
{
    if(!logging())
        return NE::kReturnSuccess;
    //create directory
    if(m_LogFolderName.isEmpty())
    {
        QDir directory(scenePath);
        directory.mkdir("Log");
        m_LogFolderName = QDateTime::currentDateTime().toString(Qt::ISODate);
        m_LogFolderName.replace(tr(":"), tr("-"));
        directory.setPath(scenePath + "/Log");
        directory.mkdir(m_LogFolderName);
        writeMainFile(scenePath + "/Log/" + m_LogFolderName);
    }

    if(!requiresLog(100.f, elapsedTime))
        return NE::kReturnSuccess;

    QString dirPath = scenePath + "/Log/" + m_LogFolderName + "/";

    if(m_LoggingTop && requiresLog(m_LogsPerSecondTop))
    {
        writeFile(messagesTop, dirPath + "osdTop.log");
    }

    if(m_LoggingTopRight && requiresLog(m_LogsPerSecondTopRight))
    {
        writeFile(messagesTopRight, dirPath + "osdTopRight.log");
    }

    if(m_LoggingRight && requiresLog(m_LogsPerSecondRight))
    {
        writeFile(messagesRight, dirPath + "osdRight.log");
    }

    if(m_LoggingBottomRight && requiresLog(m_LogsPerSecondBottomRight))
    {
        writeFile(messagesBottomRight, dirPath + "osdBottomRight.log");
    }

    if(m_LoggingBottom && requiresLog(m_LogsPerSecondBottom))
    {
        writeFile(messagesBottom, dirPath + "osdBottom.log");
    }

    if(m_LoggingBottomLeft && requiresLog(m_LogsPerSecondBottomLeft))
    {
        writeFile(messagesBottomLeft, dirPath + "osdBottomLeft.log");
    }

    if(m_LoggingLeft && requiresLog(m_LogsPerSecondLeft))
    {
        writeFile(messagesLeft, dirPath + "osdLeft.log");
    }

    if(m_LoggingTopLeft && requiresLog(m_LogsPerSecondTopLeft))
    {
        writeFile(messagesTopLeft, dirPath + "osdTopLeft.log");
    }

    m_LogsRecorded++;
    return NE::kReturnSuccess;
}

uint NEOsd::writeMainFile(QString path)
{
    QFile file(path + "/main.log");
    if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        return NE::kReturnFail;
    QTextStream out(&file);
    out.setCodec("UTF-8");

    //uint strlength = 256;
    QString compname = "TmpComputer";
    //LPSTR compname = new CHAR[strlength];
    //GetComputerNameA(compname, &strlength);
    QString machineName(compname);
    //delete [] compname;

    out << "Machine: " << machineName << endl;
    out << "Threads: " << NEBASE.Threads() << endl;
    out << "Starting Time: " << QTime::currentTime().toString() << endl;
    out << "Date: " << QDate::currentDate().toString() << endl;
    //out<<"Log started at frame: "<< MAIN.m_pScene->m_TimeStep <<endl;
    //out<<"Collider: "<<endl;

    return NE::kReturnSuccess;
}

uint NEOsd::logHeadlines(const QMap<uint, NEOsdMsg>& messages, QTextStream& out)
{

    for(QMap<uint, NEOsdMsg>::const_iterator msgIter = messages.begin();
        msgIter != messages.end(); ++ msgIter)
    {
        const QString* m_ItemName = &msgIter.value().m_ItemName;

        out <<  *m_ItemName << "\t";
    }
    out << "\n\n";
    return NE::kReturnSuccess;
}

uint NEOsd::writeFile(const QMap<uint, NEOsdMsg>& messages, QString fileName)
{
    QFile file(fileName);

    bool fileExist = true;
    if(!file.exists())
        fileExist = false;

    if(!file.open(QFile::Append | QFile::Text))
        return NE::kReturnFail;
    QTextStream out(&file);
    out.setCodec("UTF-8");

    if(!fileExist)
        logHeadlines(messages, out);

    for(QMap<uint, NEOsdMsg>::const_iterator msgIter = messages.begin();
        msgIter != messages.end(); ++ msgIter)
    {
        const NEOsdMsg& data = msgIter.value();
        out <<  data.m_ItemText << "\t";
    }
    out << "\n";
    return NE::kReturnSuccess;
}

/*!
*/
QMap<uint, NEOsdMsg>* NEOsd::getPositionMap(const NEObject::NEOsdMsgPos position)
{
    QMap<uint, NEOsdMsg>* pPositionMap = NULL;
    switch(position)
    {
    case NEObject::Top:
        pPositionMap = &messagesTop;
        break;
    case NEObject::TopRight:
        pPositionMap = &messagesTopRight;
        break;
    case NEObject::Right:
        pPositionMap = &messagesRight;
        break;
    case NEObject::BottomRight:
        pPositionMap = &messagesBottomRight;
        break;
    case NEObject::Bottom:
        pPositionMap = &messagesBottom;
        break;
    case NEObject::BottomLeft:
        pPositionMap = &messagesBottomLeft;
        break;
    case NEObject::Left:
        pPositionMap = &messagesLeft;
        break;
    case NEObject::TopLeft:
    default:
        pPositionMap = &messagesTopLeft;
        break;
    }
    return pPositionMap;
}

uint NEOsd::getOffsety() const
{
    return offsety;
}

void NEOsd::setOffsety(const uint& value)
{
    offsety = value;
}


/*!
*/
void NEOsd::moveBuffer(const NEObject::NEOsdMsgPos oldPos, const NEObject::NEOsdMsgPos newPos, const NEObject* object)
{
    if(oldPos == newPos)
        return;

    QMap<uint, NEOsdMsg>* pOldMap = getPositionMap(oldPos);
    QMap<uint, NEOsdMsg>* pNewMap = getPositionMap(newPos);
    //copy content
    if(pOldMap && pNewMap)
    {
        uint startRank = object->osdStartRank();
        uint endRank = startRank + NEOsdMsg::m_NrOfOsdMessagesPerObject;

        QMutableMapIterator<uint, NEOsdMsg> msgIter(*pOldMap);

        while(msgIter.hasNext())
        {
            msgIter.next();
            if(msgIter.key() >= startRank && msgIter.key() < endRank)
            {
                uint key = msgIter.key();
                NEOsdMsg msg = pOldMap->take(key);
                pNewMap->insert(key, msg);
            }
        }
    }
}

/*!
*/
void NEOsd::clearBuffer(const NEObject::NEOsdMsgPos position)
{
    QMap<uint, NEOsdMsg>* pPositionMap = getPositionMap(position);
    if(pPositionMap)
        pPositionMap->clear();
    else
        Q_ASSERT_X(false, "NEOsd::clearBuffer", "No such OSD exists");
}
