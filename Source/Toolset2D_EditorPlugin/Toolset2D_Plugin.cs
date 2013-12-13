using System;
using System.IO;
using System.Windows.Forms;
using System.Linq;
using CSharpFramework;
using CSharpFramework.Serialization;
using CSharpFramework.Contexts;
using CSharpFramework.Shapes;
using System.Diagnostics;
using CSharpFramework.Math;
using System.Drawing;
using CSharpFramework.Actions;
using ManagedFramework;
using ManagedBase;
using CSharpFramework.AssetManagement;
using Toolset2D_Managed;

namespace Toolset2D
{
    #region IDropContext

    public class IconManager
    {
        private static int m_SpriteIndex = -1;
        private static int m_2DIndex = -1;
        private static int m_CameraIndex = -1;

        private static void Initialize()
        {            
            if (m_SpriteIndex == -1)
            {
                m_SpriteIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Toolset2D.Resources.sprite, "SpriteIcon", Color.Magenta);
                m_2DIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Toolset2D.Resources._2d, "Category2DIcon", Color.Magenta);
                m_CameraIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Toolset2D.Resources.camera, "CameraIcon", Color.Magenta);
            }
        }

        public static int CategoryIndex
        {
            get
            {
                Initialize();
                return m_2DIndex;
            }
        }

        public static int CameraIndex
        {
            get
            {
                Initialize();
                return m_CameraIndex;
            }
        }

        public static int SpriteIndex
        {
            get
            {
                Initialize();
                return m_SpriteIndex;
            }
        }
    }

    /// <summary>
    /// Base class of a drag and drop context
    /// </summary>
    public class SpriteDropContext : IDropContext
    {
        #region Dragging functions

        public struct SpriteDropData
        {
            public string _sheet;
            public string _xml;
            public string _name;
        }

        static public Nullable<SpriteDropData> GetData(System.Windows.Forms.DragEventArgs e)
        {
            SpriteDropData data;

            data._sheet = "";
            data._xml = "";
            data._name = "";

            Nullable<SpriteDropData> result = null;

            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] filenames = e.Data.GetData(DataFormats.FileDrop, true) as string[];

                string[] extensions = ImageLoader.GetSupportedFileExtensions().ToArray();
                foreach (string filename in filenames)
                {
                    string extension = Path.GetExtension(filename);
                    if (string.IsNullOrEmpty(data._sheet) && extensions.Contains(extension))
                    {
                        data._sheet = filename;
                        data._name = Path.GetFileNameWithoutExtension(filename);
                    }
                    if (string.IsNullOrEmpty(data._xml) && extension == ".xml")
                    {
                        data._xml = filename;
                    }
                }

                result = data;
            }
            else if (e.Data.GetDataPresent("application/assetPath"))
            {
                string[] assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
                string[] assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);

                for (int i = 0; i < assetPaths.Length; i++)
                {
                    if (assetTypes[i] == "Texture")
                    {
                        data._sheet = assetPaths[i];
                        data._name = Path.GetFileNameWithoutExtension(assetPaths[i]);
                        result = data;
                        break;
                    }
                }
            }

            return result;
        }

        private void CreateShape(System.Windows.Forms.DragEventArgs e)
        {
            Nullable<SpriteDropData> data = GetData(e);

            if (data != null)
            {
                SpriteShape entity = new SpriteShape(data.Value._name);
                entity.SpriteSheetFilename = data.Value._sheet;
                entity.ShoeBoxData = data.Value._xml;
                entity.SetHint(ShapeBase.HintFlags_e.RetainPositionAtCreation, true);
                _dummyShape = entity;
            }
        }

        public override void DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
        {
            CreateShape(e);
        }

        public override void DragLeave()
        {
            if (_dummyShape != null)
            {
                _dummyShape.Dispose();
                _dummyShape = null;
            }

            EditorManager.ActiveView.UpdateView(false);
        }

        public override void DragOver(object sender, System.Windows.Forms.DragEventArgs e)
        {
            if (_dummyShape != null)
            {
                _dummyShape.Position = GetPosition(e);
            }

            EditorManager.ActiveView.UpdateView(true);
        }

        public override void DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
        {
            CreateShape(e);

            // Clones the dummy and places it correctly into the scene.
            EditorManager.ActiveView.DropObject(_dummyShape, e);
            EditorManager.ActiveView.Focus();

            Vector3F position = GetPosition(e);
            SpriteShape newShape = EditorManager.ActiveView.Gizmo.Shapes[0] as SpriteShape;
            newShape.SetCenterPosition(position.X, position.Y);
            EditorManager.ActiveView.Gizmo.Position = position;
   
            //EditorManager.ActiveView.Gizmo

            // Cleanup
            DragLeave();
        }

        private Vector3F GetPosition(System.Windows.Forms.DragEventArgs e)
        {
            Point p = new Point(e.X, e.Y);
            Point relP = EditorManager.ActiveView.PointToClient(p);
            return new Vector3F(relP.X, relP.Y, 0);
        }
        #endregion

        #region Member variables

        private SpriteShape _dummyShape;
        #endregion
    }

    #endregion

    /// <summary>
    /// EditorPlugin : this class must exist and implement the IEditorPluginModule functions to add the shape creators. The
    /// namespace (Toolset2D) must be the same as the plugin dll name, in this case Toolset2D.EditorPlugin.dll
    /// This is because the editor plugin manager searches for the Toolset2D.EditorPlugin class when opening the 
    /// Toolset2D.EditorPlugin.dll assembly
    /// </summary>
    public class EditorPlugin : IEditorPluginModule
    {
        /// <summary>
        /// Constructor of EditorPlugin that sets up plugin information
        /// </summary>
        public EditorPlugin()
        {
            // Used for serialization
            _version = 1;

            _name = "Toolset2D";
        }

        /// <summary>
        /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
        /// This global info is filled inside InitPluginModule
        /// </summary>
        public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

        public void QueryDragDropContext(object sender, QueryDragDropContextArgs e)
        {
            if (SpriteDropContext.GetData(e) != null)
            {
                e._context = new SpriteDropContext();
            }
        }

        /// <summary>
        /// InitPluginModule : called at plugin initialization time: Add the relevant shape m_shapeCreators here
        /// </summary>
        public override bool InitPluginModule()
        {
            Toolset2D_Managed.ManagedModule.InitManagedModule();

            EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Toolset2D" };

            m_shapeCreators = new IShapeCreatorPlugin[]
                 {
                   new SpriteShapeCreator(),
                   new Camera2dShapeCreator()
                 };

            // add them to the editor
            foreach (IShapeCreatorPlugin plugin in m_shapeCreators)
            {
                EditorManager.ShapeCreatorPlugins.Add(plugin);
            }
            
            EditorManager.QueryDragDropContext += new QueryDragDropContextEventHandler(QueryDragDropContext);

            return true;
        }

        /// <summary>
        /// DeInitPluginModule : called at plugin de-initialization time: Remove relevant data
        /// </summary>
        public override bool DeInitPluginModule()
        {
            // deregister the creator plugins again
            foreach (IShapeCreatorPlugin plugin in m_shapeCreators)
            {
                EditorManager.ShapeCreatorPlugins.Remove(plugin);
            }

            EditorManager.QueryDragDropContext -= new QueryDragDropContextEventHandler(QueryDragDropContext);

            Toolset2D_Managed.ManagedModule.DeInitManagedModule();

            return true;
        }

        /// <summary>
        /// Overridable that determines whether this is a standard plugin that is always loaded and initialized
        /// </summary>
        public override bool IsStandardPlugin
        {
            get
            {
                return true;
            }
        }

        IShapeCreatorPlugin[] m_shapeCreators;
    }
}

