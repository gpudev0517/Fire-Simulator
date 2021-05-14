#pragma once

#include "NECamera.h"

class NECamera;

class NECameraPrivate : public QObject
{
	Q_OBJECT

public:
	NECameraPrivate(NECamera* s);

	//! Constructs the look-at matrix using the supplied information.
    void computeLookAtMatrix();
	//! Updates the look-at matrix
	void updateLookAtMatrix();
	//! Computes the projection matrix using the current available information.
    void computeProjection(int width = 0, int height = 0);
    //! Computes the view matrix using the current available information. This should be called once per every trackball manipulation.
	void computeViewer();
	//! Updates the viewer
    void updateViewer();

	double m_OrthoWidth;         /**< Orthoraphic width of the cam */
	double m_Fov;                  /**< Field of view */
	double m_NearClip;             /**< Near clipping plane */
	double m_FarClip;                /**< Far clipping plane */
	vec3f m_LookAt;             /**< Look at */
	vec3f m_Side;                   /**< Side std::vector (normalized) */
	vec3f m_Up;                     /**< Up std::vector (normalized) */
	vec3f m_Look;                   /**< Look-at std::vector (normalized) */
	bool m_IsViewer;                /**< Is camera the viewer */
	matrix44f m_Projection;         /**< The projection matrix */
	matrix44f m_ViewTransform;      /**< The view transformation matrix */
	matrix33f m_ViewRotation;       /**< The view matrix's pure rotational term (before the translation) */

    bool m_FlipSide;                /**< Flips side std::vector */
	bool m_FlipUp;                 /**< Flips up std::vector */
	bool m_FlipLook;                /**< Flips look std::vector */

    float m_lastTheta;
    bool m_isFlipped;

    bool m_Ortho = false;           /**< Orthographic Camera */

    NECamera::OrthographicType m_OrthoType;

	NEIndexedTriMesh* m_Mesh;

private:
	NECamera* S;

};
