using System;
using CSharpFramework;
using CSharpFramework.Serialization;

namespace Toolset2D
{
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

