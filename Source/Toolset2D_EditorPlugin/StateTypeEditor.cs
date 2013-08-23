using System;
using System.Drawing.Design;
using System.Windows.Forms;
using System.Windows.Forms.PropertyGridInternal;
using System.Windows.Forms.Design;
using System.ComponentModel;
using System.Drawing;

namespace Toolset2D
{
    #region StateTypeEditor

    /// <summary>
    /// Implements a custom type editor for selecting states from a list.
    /// </summary>
    public class StateTypeEditor : UITypeEditor
    {

        #region Internal Class: ListItem

        /// <summary>
        /// Internal class used for storing custom data in listviewitems
        /// </summary>
        public class ListItem
        {
            #region Members

            private string name;
            private uint enumValue;
            private string tooltip;

            #endregion Members

            #region Constructor

            /// <summary>
            /// Creates a new instance of the <c>ListItem</c>
            /// </summary>
            /// <param name="_name">The name of the enum which is return by the <c>ToString</c> method.</param>
            /// <param name="_value">The integer value of the enum</param>
            /// <param name="_tooltip">The tooltip displayed in the <see cref="ListBox"/></param>
            public ListItem(string _name, uint _value, string _tooltip)
            {
                // set the members
                this.name = _name;
                this.enumValue = _value;
                this.tooltip = _tooltip;
            }

            #endregion Constructor

            #region Properties

            /// <summary>
            /// The int value for this item
            /// </summary>
            public uint Value
            {
                get { return enumValue; }
            }

            /// <summary>
            /// The tooltip for this item
            /// </summary>
            public string Tooltip
            {
                get { return tooltip; }
            }

            #endregion Properties

            #region Object

            /// <summary>
            /// The name of this item
            /// </summary>
            /// <returns>The name passed in the constructor</returns>
            public override string ToString()
            {
                return name;
            }

            #endregion Object
        }

        #endregion Internal Class: ListItem

        #region Members

        /// <summary>
        /// Service instances passed to the EditValue function
        /// </summary>
        private IWindowsFormsEditorService editorService = null;

        /// <summary>
        ///  Instance of the listbox that is displayed as dropdown
        /// </summary>
        private ListBox listBox;

        /// <summary>
        ///  Control instance for displaying tooltips
        /// </summary>
        private ToolTip tooltipControl;

        /// <summary>
        /// Should the editing be canceled
        /// </summary>
        private bool cancelEdit;

        #endregion Members

        #region UITypeEditor overrides

        /// <summary>
        /// Overridden UITypeEditor function
        /// </summary>
        /// <param name="context"></param>
        /// <returns></returns>
        [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context != null && context.Instance != null)
                return UITypeEditorEditStyle.DropDown;
            return UITypeEditorEditStyle.None;
        }


        /// <summary>
        /// Overridable that is called to fill the combobox with data. The default implementation parses the enum values
        /// </summary>
        /// <param name="context"></param>
        /// <param name="provider"></param>
        /// <param name="value"></param>
        /// <param name="list"></param>
        public virtual void FillListBox(SpriteShape shape, ITypeDescriptorContext context, IServiceProvider provider, object value, ListBox list)
        {
            string[] names = shape.EngineNode.GetStateNames();
            string currentState = shape.EngineNode.GetCurrentState();
            foreach (string name in names)
            {
                // Creates a clbItem that stores the name, the int value and the tooltip
                int added = list.Items.Add(new ListItem(name, 0, ""));
                if (name == currentState)
                {
                    list.SelectedIndex = added;
                }
            }
        }

        /// <summary>
        /// Overridden UITypeEditor function
        /// </summary>
        /// <param name="context"></param>
        /// <param name="provider"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        [RefreshProperties(RefreshProperties.All)]
        [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            if (context != null && context.Instance != null && provider != null)
            {
                SpriteShape shape = (SpriteShape)context.Instance;
                // get the editor service
                editorService = provider.GetService(typeof(IWindowsFormsEditorService)) as IWindowsFormsEditorService;
                if (editorService != null)
                {
                    // Create a ListBox and populate it with all (allowed) the enum values
                    listBox = new ListBox();
                    listBox.BorderStyle = BorderStyle.None;
                    listBox.MouseUp += new MouseEventHandler(this.OnMouseUp);
                    listBox.MouseMove += new MouseEventHandler(this.OnMouseMoved);
                    listBox.KeyDown += new KeyEventHandler(this.OnKeyDown);

                    // create a tooltip
                    tooltipControl = new ToolTip();
                    tooltipControl.ShowAlways = true;

                    FillListBox(shape, context, provider, value, listBox);
                    
                    listBox.IntegralHeight = false;
                    int items = listBox.Items.Count;
                    if (items > 10)
                        items = 10;
                    listBox.Height = items * listBox.ItemHeight;

                    // reset cancel edit
                    cancelEdit = false;

                    // Show our listbox as a DropDownControl. 
                    // this methods returns when the dropdowncontrol is closed
                    editorService.DropDownControl(listBox);

                    // if the editing has been canceled
                    if (cancelEdit)
                    {
                        // return the old value
                        return value;
                    }
                    
                    // return the selected enum value
                    return listBox.SelectedItem.ToString();
                }
            }

            return value;
        }

        #endregion UITypeEditor overrides

        #region Key events

        /// <summary>
        /// Cancel the editing when ESC is pressed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            // if the ESC key has been pressed
            if (e.KeyCode == Keys.Escape)
            {
                // cancel the editing
                cancelEdit = true;
                e.Handled = true;
            }
        }

        #endregion Key events

        #region Mouse events

        /// <summary>
        /// Close the dropdown control when the mouse is clicked
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            // if there is an editor service
            if (editorService != null)
            {
                // remove the tooltip
                tooltipControl.RemoveAll();

                // close the dropdown control
                editorService.CloseDropDown();
            }
        }

        string _lastTooltipString = null;
        /// <summary>
        /// Sets the tooltip of the item under the mouse pointer when the mouse is moved
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnMouseMoved(object sender, MouseEventArgs e)
        {
            // Get the currently selected item
            int index = listBox.IndexFromPoint(e.X, e.Y);
            if (index >= 0)
            {
                // set the tooltip if something is selected
                string newValue = ((ListItem)listBox.Items[index]).Tooltip;

                if (newValue != _lastTooltipString)
                {
                    tooltipControl.SetToolTip(listBox, newValue);
                    _lastTooltipString = newValue;
                }
            }
        }

        #endregion Mouse events

    }

    #endregion StateTypeEditor
}
