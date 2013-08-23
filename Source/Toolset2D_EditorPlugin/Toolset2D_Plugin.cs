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

    /// <summary>
    /// Base class of a drag and drop context
    /// </summary>
    public class SpriteDropContext : IDropContext
    {
        #region Dragging functions

        private void createShape(System.Windows.Forms.DragEventArgs e)
        {
            _filenames = e.Data.GetData(DataFormats.FileDrop, true) as string[];
            _sheet = "";
            _xml = "";
            _name = "";

            string[] extensions = ImageLoader.GetSupportedFileExtensions().ToArray();
            foreach (string filename in _filenames)
            {
                string extension = Path.GetExtension(filename);
                if (string.IsNullOrEmpty(_sheet) && extensions.Contains(extension))
                {
                    _sheet = filename;
                    _name = Path.GetFileNameWithoutExtension(filename);
                }
                if (string.IsNullOrEmpty(_xml) && extension == ".xml")
                {
                    _xml = filename;
                }
            }


            SpriteShape entity = new SpriteShape(_name);
            entity.SpriteSheetFilename = _sheet;
            entity.ShoeBoxData = _xml;
            entity.SetHint(ShapeBase.HintFlags_e.RetainPositionAtCreation, true);
            _dummyShape = entity;
        }

        public override void DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
        {
            createShape(e);
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
            createShape(e);

            // Clones the dummy and places it correctly into the scene.
            EditorManager.ActiveView.DropObject(_dummyShape, e);
            EditorManager.ActiveView.Focus();

            Shape3D newShape = EditorManager.ActiveView.Gizmo.Shapes[0] as Shape3D;
            newShape.Position = EditorManager.ActiveView.Gizmo.Position = GetPosition(e);
   
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

        private string[] _filenames;
        private string _sheet;
        private string _xml;
        private string _name;
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

        /// <summary>
        /// InitPluginModule : called at plugin initialization time: Add the relevant shape m_shapeCreators here
        /// </summary>
        public override bool InitPluginModule()
        {
            Toolset2D_Managed.ManagedModule.InitManagedModule();

            EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Toolset2D" };

            m_shapeCreators = new IShapeCreatorPlugin[]
                 {
                   new SpriteShapeCreator()
                 };

            // add them to the editor
            foreach (IShapeCreatorPlugin plugin in m_shapeCreators)
            {
                EditorManager.ShapeCreatorPlugins.Add(plugin);
            }
            
            EditorManager.QueryDragDropContext += new QueryDragDropContextEventHandler(QueryDragDropContext);

            return true;
        }

        public void QueryDragDropContext(object sender, QueryDragDropContextArgs e)
        {
            bool dragdrop = false;
            if (e.Data.GetDataPresent(DataFormats.FileDrop, true))
            {
                string[] filenames = e.Data.GetData(DataFormats.FileDrop, true) as string[];
                string[] extensions = ImageLoader.GetSupportedFileExtensions().ToArray();

                foreach (string filename in filenames)
                {
                    string extension = Path.GetExtension(filename);
                    if (extensions.Contains(extension))
                    {
                        dragdrop = true;
                        break;
                    }
                }
            }

            if (dragdrop)
            {
                e._context = new SpriteDropContext();
            }
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

