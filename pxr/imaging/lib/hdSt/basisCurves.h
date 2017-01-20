//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef HDST_BASIS_CURVES_H
#define HDST_BASIS_CURVES_H

#include "pxr/imaging/hd/version.h"
#include "pxr/imaging/hd/basisCurves.h"
#include "pxr/imaging/hd/drawingCoord.h"
#include "pxr/imaging/hd/enums.h"
#include "pxr/imaging/hd/perfLog.h"

#include "pxr/usd/sdf/path.h"
#include "pxr/base/vt/array.h"

#include <boost/shared_ptr.hpp>

/// \class HdStBasisCurvesReprDesc
///
/// Descriptor to configure a drawItem for a repr.
///
struct HdStBasisCurvesReprDesc {
    HdStBasisCurvesReprDesc(
        HdBasisCurvesGeomStyle geomStyle = HdBasisCurvesGeomStyleInvalid)
        : geomStyle(geomStyle)
        {}

    HdBasisCurvesGeomStyle geomStyle:2;
};

typedef boost::shared_ptr<class HdSt_BasisCurvesTopology>
                                              HdSt_BasisCurvesTopologySharedPtr;

/// \class HdStBasisCurves
///
/// A collection of curves using a particular basis.
///
class HdStBasisCurves final: public HdBasisCurves {
public:
    HF_MALLOC_TAG_NEW("new HdStBasisCurves");
    HdStBasisCurves(HdSceneDelegate* delegate, SdfPath const& id,
                    SdfPath const& instancerId = SdfPath());
    virtual ~HdStBasisCurves();

    /// Configure geometric style of drawItems for \p reprName
    static void ConfigureRepr(TfToken const &reprName,
                              HdStBasisCurvesReprDesc desc);

    /// Return the dirtyBits mask to be tracked for \p reprName
    static int GetDirtyBitsMask(TfToken const &reprName);

    /// Returns whether refinement is always on or not.
    static bool IsEnabledForceRefinedCurves();
    
protected:
    virtual HdReprSharedPtr const & _GetRepr(
        TfToken const &reprName, HdChangeTracker::DirtyBits *dirtyBits);

    void _PopulateTopology(HdDrawItem *drawItem,
                           HdChangeTracker::DirtyBits *dirtyBits,
                           const HdStBasisCurvesReprDesc &desc);

    void _PopulateVertexPrimVars(HdDrawItem *drawItem,
                                 HdChangeTracker::DirtyBits *dirtyBits);

    void _PopulateElementPrimVars(HdDrawItem *drawItem,
                                  HdChangeTracker::DirtyBits *dirtyBits);

    HdChangeTracker::DirtyBits _PropagateDirtyBits(
        HdChangeTracker::DirtyBits dirtyBits);

    virtual HdChangeTracker::DirtyBits _GetInitialDirtyBits() const final override;

private:
    enum DrawingCoord {
        HullTopology = HdDrawingCoord::CustomSlotsBegin,
        InstancePrimVar  // has to be at the very end
    };

    enum DirtyBits {
        DirtyIndices        = HdChangeTracker::CustomBitsBegin,
        DirtyHullIndices    = (DirtyIndices       << 1),
    };

    /// We only support drawing smooth curves for a small subset of all the
    /// curves that hydra needs to support
    /// We will fallback to line segments for unsupported curves.
    bool _SupportsSmoothCurves(const HdStBasisCurvesReprDesc &desc,
                               int refineLevel);

    void _UpdateDrawItem(HdDrawItem *drawItem,
                         HdChangeTracker::DirtyBits *dirtyBits,
                         const HdStBasisCurvesReprDesc &desc);

    void _UpdateDrawItemGeometricShader(HdDrawItem *drawItem,
                                        const HdStBasisCurvesReprDesc &desc);

    void _SetGeometricShaders();

    void _ResetGeometricShaders();

    HdSt_BasisCurvesTopologySharedPtr _topology;
    HdTopology::ID _topologyId;
    int _customDirtyBitsInUse;
    int _refineLevel;  // XXX: could be moved into HdBasisCurveTopology.

    typedef _ReprDescConfigs<HdStBasisCurvesReprDesc> _BasisCurvesReprConfig;
    static _BasisCurvesReprConfig _reprDescConfig;
};

#endif // HDST_BASIS_CURVES_H
