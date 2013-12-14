using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Drawing.Design;
using System.Collections.Generic;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Controls;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using Toolset2D_Managed;
using System.Runtime.Serialization;
using ManagedFramework;
using System.IO;

namespace Toolset2D
{
    #region class SpriteShape
    /// <summary>
    /// SpriteShape : This is the class that represents the shape in the editor. It has an engine instance that handles the
    /// native code. The engine instance code in located in the Toolset2D_Managed project (managed C++ class library)
    /// </summary>
    [Serializable]
    public class SpriteShape : ShapeObject3D
    {
        /// <summary>
        /// Category string
        /// </summary>
        protected const string CAT_SPRITE = "Sprite";
        protected const string CAT_DYNAMICS = "Dynamics";

        /// <summary>
        /// Category ID
        /// </summary>
        protected const int CATORDER_SPRITE = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

        #region Constructor

        /// <summary>
        /// The constructor of the node shape, just takes the node name
        /// </summary>
        /// <param name="name">Name of the shape in the shape tree</param>
        public SpriteShape(string name)
            : base(name)
        {
            m_collision = true;
            AddHint(HintFlags_e.HideGizmo);
        }

        #endregion

        #region Icon

        /// <summary>
        /// Return the icon index in the image list for the shape tree view. For this, use a static variable for this class.
        /// </summary>
        public override int GetIconIndex()
        {
            return IconManager.SpriteIndex;
        }

        /// <summary>
        /// Gets the icon index for selected icons in the tree view. Simply pass the icon index
        /// </summary>
        /// <returns></returns>
        public override int GetSelectedIconIndex()
        {
            return IconManager.SpriteIndex;
        }

        #endregion

        #region Engine Instance

        /// <summary>
        /// Function to create the engine instance of the shape. The engine instance is of type EngineInstanceSprite
        /// and located in the managed code library.
        /// </summary>
        /// <param name="bCreateChildren">relevant for the base class to create instances for children</param>
        public override void CreateEngineInstance(bool bCreateChildren)
        {
            _engineInstance = new EngineInstanceSprite();
            SetEngineInstanceBaseProperties(); // sets the position etc.
            base.CreateEngineInstance(bCreateChildren);
        }

        /// <summary>
        /// Removes the engine instance
        /// </summary>
        /// <param name="bRemoveChildren">relevant for the base class to cleanup its children</param>
        public override void RemoveEngineInstance(bool bRemoveChildren)
        {
            base.RemoveEngineInstance(bRemoveChildren);

            // nothing else to do here (_engineInstance already destroyed in base.RemoveEngineInstance)
        }

        /// <summary>
        /// Helper field to access the engine instance as casted class to perform specfic operations on it
        /// </summary>
        [Browsable(false)]
        public EngineInstanceSprite EngineNode { get { return (EngineInstanceSprite)_engineInstance; } }

        /// <summary>
        /// Overridden render function: Let the engine instance render itself and render a box
        /// </summary>
        /// <param name="view"></param>
        /// <param name="mode"></param>
        public override void RenderShape(VisionViewBase view, ShapeRenderMode mode)
        {
            if (HasEngineInstance())
            {
                EngineNode.RenderShape(view, mode);
            }
        }

        /// <summary>
        /// Set all properties on the engine instance. The base implementation already sets position and orientation,
        /// so there is nothing else to do here
        /// </summary>
        public override void SetEngineInstanceBaseProperties()
        {
            base.SetEngineInstanceBaseProperties();
            if (HasEngineInstance())
            {
                EngineNode.SetSpriteSheetData(m_SpriteSheetFilename, m_ShoeBoxFilename);
                EngineNode.SetFullscreenMode(m_fullscreen);
                EngineNode.SetScroll(m_scrollX, m_scrollY);
                EngineNode.SetCurrentState(m_state);
                EngineNode.SetPlayOnce(m_playOnce);
                EngineNode.SetCollision(m_collision);

                if (m_width != 0.0)
                {
                    EngineNode.SetWidth(m_width);
                }

                if (m_height != 0.0)
                {
                    EngineNode.SetHeight(m_height);
                }

                Vector3F orientation = new Vector3F();
                EngineNode.GetOrientation(ref orientation);
                EngineNode.SetOrientation(orientation.X, orientation.Y, m_rotation);

                EngineNode.SetConvexHullCollision(m_convexHullCollision);
                EngineNode.SetSimulate(m_simulate, m_fixed);
            }
        }

        /// <summary>
        /// Pick the shape in the view. Use helper function trace the local bounding box.
        /// </summary>
        /// <param name="rayStart">trace ray start</param>
        /// <param name="rayEnd">trace ray end</param>
        /// <param name="result">esult structure to fill in</param>
        public override void OnTraceShape(ShapeTraceMode_e mode, Vector3F rayStart, Vector3F rayEnd, ref ShapeTraceResult result)
        {
            if (HasEngineInstance() && mode == ShapeTraceMode_e.ShapePicking)
            {
                EngineNode.TraceShape(this, rayStart, rayEnd, ref result);
            }
        }        
        #endregion

        #region Serialization and Export

        /// <summary>
        /// Called when deserializing
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        protected SpriteShape(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            if (SerializationHelper.HasElement(info, "_sheet_filename"))
            {
                SpriteSheetFilename = info.GetString("_sheet_filename");
                ShoeBoxData = info.GetString("_shoebox_filename");
                m_state = info.GetString("_state");
                m_scrollX = info.GetSingle("_scroll_x");
                m_scrollY = info.GetSingle("_scroll_y");
                m_fullscreen = info.GetBoolean("_fullscreen");
            }

            if (SerializationHelper.HasElement(info, "_collision"))
            {
                m_collision = info.GetBoolean("_collision");
            }

            if (SerializationHelper.HasElement(info, "_rotation"))
            {
                m_rotation = info.GetSingle("_rotation");
            }

            if (SerializationHelper.HasElement(info, "_width"))
            {
                m_width = info.GetSingle("_width");
                m_height = info.GetSingle("_height");
            }

            if (SerializationHelper.HasElement(info, "_convexHullCollision"))
            {
                m_convexHullCollision = info.GetBoolean("_convexHullCollision");
            }

            if (SerializationHelper.HasElement(info, "_simulate"))
            {
                m_simulate = info.GetBoolean("_simulate");
                m_fixed = info.GetBoolean("_fixed");
            }
        }

        /// <summary>
        /// Called when serializing
        /// </summary>
        /// <param name="info"></param>
        /// <param name="context"></param>
        public override void GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
        {
            base.GetObjectData(info, context);
            info.AddValue("_sheet_filename", m_SpriteSheetFilename);
            info.AddValue("_shoebox_filename", m_ShoeBoxFilename);
            info.AddValue("_state", m_state);
            info.AddValue("_scroll_x", m_scrollX);
            info.AddValue("_scroll_y", m_scrollY);
            info.AddValue("_width", m_width);
            info.AddValue("_height", m_height);
            info.AddValue("_fullscreen", m_fullscreen);
            info.AddValue("_collision", m_collision);
            info.AddValue("_rotation", m_rotation);
            info.AddValue("_convexHullCollision", m_convexHullCollision);
            info.AddValue("_simulate", m_simulate);
            info.AddValue("_fixed", m_fixed);
        }

        /// <summary>
        /// Called during export to collect native plugin information. In this case, return the global instance that applies for all shapes in this plugin
        /// </summary>
        /// <returns></returns>
        public override CSharpFramework.Serialization.EditorPluginInfo GetPluginInformation()
        {
            return EditorPlugin.EDITOR_PLUGIN_INFO;
        }

        /// <summary>
        /// Called when exporting the scene to engine archive. base implementation calls function on engine object which in turn
        /// serializes itself
        /// </summary>
        /// <returns></returns>
        public override bool OnExport(CSharpFramework.Scene.SceneExportInfo info)
        {
            return base.OnExport(info);
        }

        public override void OnDeserialization()
        {
            base.OnDeserialization();
            AddHint(HintFlags_e.HideGizmo);
        }

        public void SetCenterPosition(float x, float y)
        {
            if (HasEngineInstance())
            {
                EngineNode.SetCenterPosition(x, y);

                Vector3F position = new Vector3F();
                EngineNode.GetPosition(ref position);

                Position = position;
            }
        }
        #endregion

        #region Properties
        public Vector2F CenterPosition
        {
            get 
            {
                Vector2F position = new Vector2F();
                if (HasEngineInstance())
                {
                    position = EngineNode.GetCenterPosition();
                }
                return position;
            }
        }

        string m_SpriteSheetFilename;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE), PropertyOrder(1)]
        [EditorAttribute(typeof(AssetEditor), typeof(UITypeEditor)), AssetDialogFilter(new string[] { "Texture" })]
        [Description("Texture used for the sprite sheet.")]
        public string SpriteSheetFilename
        {
            get { return m_SpriteSheetFilename; }
            set
            {
                if (m_SpriteSheetFilename == value) return;
                m_SpriteSheetFilename = value;
                SetEngineInstanceBaseProperties();
            }
        }

        string m_ShoeBoxFilename;
        [PrefabResolveFilename]
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(2),
        EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)),
        FileDialogFilter(new string[] { ".xml" })]
        [Description("XML file output by ShoeBox that describes where the cells are in the sprite sheet")]
        [DependentProperties(new string[] { "SpriteSheetFilename" })]
        public string ShoeBoxData
        {
            get { return m_ShoeBoxFilename; }
            set
            {
                if (m_ShoeBoxFilename != value)
                {
                    m_ShoeBoxFilename = value;
                    SetEngineInstanceBaseProperties();
                }
            }
        }

        string m_state;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(3)]
        [Description("Current state")]
        [EditorAttribute(typeof(StateTypeEditor), typeof(UITypeEditor))]
        public string State
        {
            get { return EngineNode.GetCurrentState(); }
            set
            {
                m_state = value;
                SetEngineInstanceBaseProperties();
            }
        }

        float m_scrollX;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(4)]
        [Description("")]
        public float ScrollX
        {
            get { return EngineNode.GetScrollX(); }
            set
            {
                m_scrollX = value;
                SetEngineInstanceBaseProperties();
            }
        }

        float m_scrollY;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(5)]
        [Description("")]
        public float ScrollY
        {
            get { return EngineNode.GetScrollY(); }
            set
            {
                m_scrollY = value;
                SetEngineInstanceBaseProperties();
            }
        }

        float m_width;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(6)]
        [Description("")]
        public float Width
        {
            get { return EngineNode.GetWidth(); }
            set
            {
                m_width = value;
                SetEngineInstanceBaseProperties();
            }
        }

        float m_height;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(7)]
        [Description("")]
        public float Height
        {
            get { return EngineNode.GetHeight(); }
            set
            {
                m_height = value;
                SetEngineInstanceBaseProperties();
            }
        }

        bool m_playOnce;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(8)]
        [Description("PlayOnce")]
        public bool PlayOnce
        {
            get { return EngineNode.IsPlayOnce(); }
            set
            {
                m_playOnce = value;
                SetEngineInstanceBaseProperties();
            }
        }
        
        bool m_collision;
        [SortedCategory(CAT_DYNAMICS, CATORDER_SPRITE),
        PropertyOrder(9)]
        [Description("Collide")]
        public bool Collide
        {
            get { return EngineNode.IsColliding(); }
            set
            {
                m_collision = value;
                SetEngineInstanceBaseProperties();
            }
        }

        bool m_convexHullCollision;
        [SortedCategory(CAT_DYNAMICS, CATORDER_SPRITE),
        PropertyOrder(9)]
        [Description("AABB Collision")]
        public bool ConvexHullCollision
        {
            get { return EngineNode.IsConvexHullCollision(); }
            set
            {
                m_convexHullCollision = value;
                SetEngineInstanceBaseProperties();
            }
        }

        bool m_simulate;
        [SortedCategory(CAT_DYNAMICS, CATORDER_SPRITE),
        PropertyOrder(9)]
        [Description("Simulate")]
        public bool Simulate
        {
            get { return EngineNode.IsSimulated(); }
            set
            {
                m_simulate = value;
                SetEngineInstanceBaseProperties();
            }
        }

        bool m_fixed;
        [SortedCategory(CAT_DYNAMICS, CATORDER_SPRITE),
        PropertyOrder(9)]
        [Description("Fixed")]
        public bool Fixed
        {
            get { return EngineNode.IsFixed(); }
            set
            {
                m_fixed = value;
                SetEngineInstanceBaseProperties();
            }
        }

        bool m_fullscreen;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(10)]
        [Description("Fullscreen")]
        public bool Fullscreen
        {
            get { return EngineNode.IsFullscreenMode(); }
            set
            {
                m_fullscreen = value;
                SetEngineInstanceBaseProperties();
            }
        }

        float m_rotation;
        [SortedCategory(CAT_SPRITE, CATORDER_SPRITE),
        PropertyOrder(11)]
        [Description("")]
        public float Rotation
        {
            get
            { 
                Vector3F orientation = new Vector3F();
                EngineNode.GetOrientation(ref orientation);
                return orientation.Z; 
            }
            set
            {
                m_rotation = value;
                SetEngineInstanceBaseProperties();
            }
        }
        #endregion


        #region Hotspot

        HotSpot2D _hotSpotMove;        // a hotspot for the BoxSize X

        /// <summary>
        /// Called when the shape is selected
        /// </summary>
        public override void OnSelected()
        {
            base.OnSelected();
            System.Diagnostics.Debug.Assert(_hotSpotMove == null);

            // create the hotspots:

            // hotspot for size X
            _hotSpotMove = new HotSpot2D(this, 20.0f); //@"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 4.0f);
            _hotSpotMove.ToolTipText = "Move";
            EditorManager.ActiveView.HotSpots.Add(_hotSpotMove);

            UpdateHotspotVisibility();

            // add more hotspots here...
        }


        void SafeRemoveHotSpot(HotSpotBase hotSpot)
        {
            if (hotSpot == null) return;
            EditorManager.ActiveView.HotSpots.Remove(hotSpot);
            hotSpot.Remove();
        }


        void UpdateHotspotVisibility()
        {
        }


        void UpdateTypeFlags()
        {
        }

        /// <summary>
        /// Called when the shape is unselected
        /// </summary>
        public override void OnUnSelected()
        {
            SafeRemoveHotSpot(_hotSpotMove); _hotSpotMove = null;
            base.OnUnSelected();
        }


        /// <summary>
        /// Called when user starts dragging a hotspot that belongs to this shape
        /// </summary>
        /// <param name="hotSpot"></param>
        /// <param name="view"></param>
        public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
        {
            if (hotSpot == _hotSpotMove)
            {
                //_hotSpotMove.StartDistance = this.BoxSizeX;
            }
        }

        /// <summary>
        /// Called while the user drags a hotspot that belongs to this shape
        /// </summary>
        /// <param name="hotSpot"></param>
        /// <param name="view"></param>
        /// <param name="fDeltaX"></param>
        /// <param name="fDeltaY"></param>
        public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
        {
            if (hotSpot == _hotSpotMove)
            {
                //this.BoxSizeX = _hotSpotMove.CurrentDistance;
            }
        }

        /// <summary>
        /// Called when the user releases the mouse button on a hotspot
        /// </summary>
        /// <param name="hotSpot"></param>
        /// <param name="view"></param>
        public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
        {
            if (hotSpot == _hotSpotMove)
            {
                //if (_hotSpotMove.HasChanged)
                //{
                //    float fNewSize = _hotSpotMove.CurrentDistance;
                //    this.BoxSizeX = _hotSpotMove.StartDistance; // set old value for the action
                //    EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "BoxSizeX", fNewSize)); // send an action which sets the property from old value to new one
                //}
            }
        }

        /// <summary>
        /// Called every frame and per hotspot that belongs to the shape
        /// </summary>
        /// <param name="hotSpot"></param>
        public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
        {
            if (hotSpot == _hotSpotMove)
            {
                //_hotSpotMove.Axis = XAxis;
                //if (!_hotSpotMove.IsDragging)
                //    _hotSpotMove.StartDistance = this.BoxSizeX;
            }
        }

        #endregion

    }
    #endregion

    #region Shape Creator Plugin


    /// <summary>
    /// Creator class. An instance of the creator is registered in the plugin init function. Thus the creator shows
    /// up in the "Create" menu of the editor
    /// </summary>
    class SpriteShapeCreator : CSharpFramework.IShapeCreatorPlugin
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public SpriteShapeCreator()
        {
            IconIndex = IconManager.SpriteIndex;
            CategoryIconIndex = IconManager.CategoryIndex;
        }

        /// <summary>
        /// Get the name of the plugin, for instance the shape name. This name appears in the "create" menu
        /// </summary>
        /// <returns>creator name</returns>
        public override string GetPluginName()
        {
            return "2D Sprite";
        }

        /// <summary>
        /// Get the plugin category name to sort the plugin name. This is useful to group m_shapeCreators. A null string can
        /// be returned to put the creator in the root
        /// </summary>
        /// <returns></returns>
        public override string GetPluginCategory()
        {
            return "2D Toolset";
        }

        /// <summary>
        /// Returns a short description text
        /// </summary>
        /// <returns></returns>
        public override string GetPluginDescription()
        {
            return "2D Sprite Entity";
        }

        public override ShapeBase CreateShapeInstance()
        {
            SpriteShape shape = new SpriteShape("Sprite");
            shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
            return shape;
        }
    }

    #endregion
}