/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QTableWidget;
class QTableWidgetItem;
class CustomScene;
class QGraphicsView;
QT_END_NAMESPACE

NE_FWD_DECL_3(class, NENode, NEColorRamp, QtVariantProperty)

#define _LIST_ICON_WIDTH_   100
#define _LIST_ICON_HEIGHT_  30
#define _LIST1_ICON_WIDTH_  200
#define _LIST1_ICON_HEIGHT_ 30

class NEColorPresetDialog : public QDialog
{
    Q_OBJECT

public:
    NEColorPresetDialog(QtVariantProperty* prop, QWidget *parent = Q_NULLPTR);
    QIcon crateColorRampPresetImage(NEColorRamp* colorRamp = Q_NULLPTR, int imageWidth = _LIST_ICON_WIDTH_, int imageHeight = _LIST_ICON_HEIGHT_);
    void initPresetListWidget();
    void updateCurColorRampAndValueTableWithoutName();
    void createValueTable();
    void setValueToTable(NEColorRamp* colorRamp);
    void getValueToTable(NEColorRamp* colorRamp);
    bool isLoadColorRampFromPresetToNode() { return m_loadColorRampFromPreset; }
    void setValueToGraphicView(NEColorRamp* colorRamp);

public slots:
    void addPreset();
    void deletePreset();
    void loadPreset();
    void applyPreset();
    void itemChanged(QListWidgetItem *item);

    void graphicsViewChanged(QList<QRectF> region);

private:
    void createItem(QtVariantProperty* prop, QListWidget* listWidget, int alignment,
                    int imageWidth = _LIST_ICON_WIDTH_, int imageHeight = _LIST_ICON_HEIGHT_, QString itemStr = "");

    QListWidget*    m_contentsWidget;
    QListWidget*    m_currentWidget;
    QGraphicsView*  m_graphicView;
    CustomScene*  m_graphicScene;
    QTableWidget*   m_valueTable;
//    NENode*         m_node;
    QtVariantProperty* m_prop;
    QMap<QString, NEColorRamp*> m_colorPresetListMap;
    NEColorRamp*    m_currentNodeColorRamp;
    bool            m_loadColorRampFromPreset;
};


#endif
