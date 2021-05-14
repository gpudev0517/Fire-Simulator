#pragma once

class NECEXP_CORE NEFrameInfo
{
public:
	NEFrameInfo();

    bool operator==(const NEFrameInfo& val) const;

	int firstFrame() const;
	void setFirstFrame(int val);

	int lastFrame() const;
	void setLastFrame(int val);

	bool completelyCached() const;
	void setCompletelyCached(bool val);

	operator QVariant() const;

private:
	//!
	int m_FirstFrame;
	//!
	int m_SimLastFrame;
	//!
	bool m_Complete;
};

Q_DECLARE_METATYPE(NEFrameInfo)
