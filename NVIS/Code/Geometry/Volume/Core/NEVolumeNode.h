#pragma once

#include "Spatial/NESpatial.h"

class NEColorRamp;

NE_FWD_DECL_2(class, NEVolume, NE_PRIVATE(NEVolumeNode))

class NECEXP_GEOMETRY NEVolumeNode : public NESpatial
{
	Q_OBJECT
	NE_CLASS("Volume Node")

public:
	enum DisplayMode
	{
		Points     = 0,
		Box        = 1,
		IsoSurface = 2
	};

	NE_PROP_RW(double, EvalStart, "Start Time", "Evaluation of Node Start Time", true,)
	NE_PROP_RW(double, EvalEnd, "End Time", "Evaluation of Node End Time", true,)
	NE_PROP_RW(DisplayMode, DispMode, "Display Mode", "Display Mode of Visualizing Volumes", true,)
	NE_PROP_RW(NEBool, Eval, "Evaluate", "Evaluate the node so that the entire tree is evaluated", true,)
	NE_PROP_RW(bool, FlipXAxis, "Flip X Axis", "Flips the X Axis.", true, virtual)
	NE_PROP_RW(bool, FlipYAxis, "Flip Y Axis", "Flips the Y Axis.", true, virtual)
	NE_PROP_RW(bool, FlipZAxis, "Flip Z Axis", "Flips the Z Axis.", true, virtual)

	Q_ENUMS(DisplayMode)

public:
	NEVolumeNode(NENodeFactory* factory);
	~NEVolumeNode();

	virtual std::shared_ptr< NEVolume > Volume();
	void SetVolume(const std::shared_ptr< NEVolume >& arg);

	uint init(uint initMode = NE::kInitOnCreate);

	//! Resets all properties to their default values
	virtual void resetProperties();

	virtual std::shared_ptr< NEVolume > inputVolume(NENode* source, uint type, int slot);

	void render();
	void renderPickable();
	void renderPicked();

	bool updateAABB();

	//! Update called during simulation update
	virtual uint update();

	//! called during node graph eval
	virtual uint eval();

	static const vec3f& colorSign(bool negative);

	virtual uint saveFrameData(QString path, const int frameNumber);
	virtual uint loadFrameData(QString path, const int frameNumber);
	virtual NEColorRamp* getColorRampForVolume()
	{
		return NULL;
	}

public slots:
	bool Active();

signals:
	bool createVisualizer(NESpatial* spatial);

private:
	NE_DECL_PRIVATE(NEVolumeNode)

protected:
	virtual void removeSource(NENode* source, uint type, uint sIndex, uint tIndex);
};
