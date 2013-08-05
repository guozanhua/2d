// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM		: ALL
// PRODUCT		: VISION
// VISIBILITY		: PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

using System;
using CSharpFramework;
using CSharpFramework.Serialization;

namespace SpriteShapeEditorPlugin
{
	/// <summary>
	/// EditorPlugin : this class must exist and implement the IEditorPluginModule functions to add the shape creators (for instance)
	/// Also, the namespace (NodeEditorPlugin) must be the same as the plugin dll name, in ths case NodeEditorPlugin.EditorPlugin.dll
  /// This is because the editor plugin manager searches for the SpriteShapeEditorPlugin.EditorPlugin class when opening the 
  /// SpriteShapeEditorPlugin.EditorPlugin.dll assembly
	/// </summary>
	public class EditorPlugin : IEditorPluginModule
	{
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                 //version used for serialization
      _name = "SpriteShapeEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();


    /// <summary>
    /// InitPluginModule : called at plugin initialisation time: Add the relevant shape creators here
    /// </summary>
    public override bool InitPluginModule()
    {
      NodeManaged.ManagedModule.InitManagedModule();

      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "SpriteShapeEditorPlugin" };

      // we only have one type of shape in this plugin
      creators = new IShapeCreatorPlugin[]
                 {
                   new NodeShapeCreator()
                 };

      // add them to the editor
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Add( plugin );
/*
      // register tests (optional)
      TestSuite suite = new  TestSuite("Node Shape Plugin Tests");
      suite.Add( new TestFixture(typeof(Tests.EntityShapeTest)) );
      TestManager.AddTestSuite( suite );
*/
      return true;
    }

    /// <summary>
    /// DeInitPluginModule : called at plugin deinitialisation time: Remove relevant data
    /// </summary>
    public override bool DeInitPluginModule()
    {
      // deregister the creator plugins again
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Remove( plugin );
      
      NodeManaged.ManagedModule.DeInitManagedModule();

      return true;
    }

    /// <summary>
    /// Overridable that determines whether this is a standard plugin that is always loaded and initialized
    /// </summary>
    public override bool IsStandardPlugin
    {
      get
      {
        return false;
      }
    }


    IShapeCreatorPlugin[] creators;
  }
}

