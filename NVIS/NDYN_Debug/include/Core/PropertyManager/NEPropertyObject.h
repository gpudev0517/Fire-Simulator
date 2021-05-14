#pragma once


class NECEXP_CORE NEPropertyObject : public QObject
{
    Q_OBJECT

public:
    //! Read dummy for group properties
    static const PropertyGroupStart groupStart() { return PropertyGroupStart(); }
    static const PropertyGroupEnd groupEnd() { return PropertyGroupEnd();}

    //! Read dummy for group properties
    static const SubPropertyStart subStart() { return SubPropertyStart(); }
    static const SubPropertyEnd subEnd() { return SubPropertyEnd();}
};
