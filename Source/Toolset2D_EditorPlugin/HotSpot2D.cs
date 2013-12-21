using System;
using CSharpFramework;
using CSharpFramework.View;
using System.Collections;
using System.Windows.Forms;
using CSharpFramework.Shapes;
using CSharpFramework.Math;

namespace Toolset2D
{
    /// <summary>
    /// This derived version allows to drag in all 6 directions
    /// </summary>
    public class HotSpot2D : HotSpotBase
    {
        #region Constructor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="owner"></param>
        /// <param name="fSize"></param>
        public HotSpot2D(SpriteShape owner, float fSize)
            : base(owner, null, VisionColors.White, HotSpotBase.PickType.Circle, fSize)
        {
            _fSize = fSize; //  *EditorManager.Settings.GlobalUnitScaling; no unit scaling since 2D
        }

        #endregion

        #region relative position

        Vector2F _startPos = Vector2F.Zero;
        Vector2F _relPos = Vector2F.Zero;
        float _fSize = 5.0f;

        public SpriteShape Sprite
        {
            get { return (Owner as SpriteShape); }
        }

        /// <summary>
        /// Gets or sets the relative 3D position
        /// </summary>
        public Vector2F CurrentPosition
        {
            get { return _relPos; }
            set { _relPos = value; }
        }

        /// <summary>
        /// Gets or sets the relative 3D position
        /// </summary>
        public Vector2F StartPosition
        {
            get { return _startPos; }
            set { _relPos = _startPos = value; }
        }

        /// <summary>
        /// Gets or sets the size of the tripod
        /// </summary>
        public float Size
        {
            get { return _fSize; }
            set { _fSize = value; }
        }

        #endregion

        #region overridden functions

        public override void RenderHotSpot(VisionViewBase view, ShapeRenderMode mode)
        {
            base.RenderHotSpot(view, mode);
            if (!OnScreen)
                return;

            Vector2F center2d = Sprite.CenterPosition;
            view.RenderRectangle2D(
                center2d.X - 5, center2d.Y - 5,
                center2d.X + 5, center2d.Y + 5,
                VisionColors.RGBA(0, 255, 0, 255), 1.0f );
        }

        public override void EvaluatePosition()
        {
            base.EvaluatePosition();
            Position = new Vector3F(Sprite.CenterPosition.X, Sprite.CenterPosition.Y, 0.0f);
        }

        public override bool IsMouseOver(float fMouseX, float fMouseY)
        {
            Vector2F mouse = new Vector2F(fMouseX, fMouseY);
            float distance = (mouse - Sprite.CenterPosition).GetLengthSqr();
            return (distance < 25);
        }

        public override void OnDragBegin(VisionViewBase view)
        {
            StartPosition = CurrentPosition = Sprite.CenterPosition;
            base.OnDragBegin(view);
        }

        public override void OnDrag(VisionViewBase view, float fDeltaX, float fDeltaY)
        {
            CurrentPosition = CurrentPosition + (new Vector2F(fDeltaX, fDeltaY));

            base.OnDrag(view, fDeltaX, fDeltaY);

            Sprite.SetCenterPosition(CurrentPosition.X, CurrentPosition.Y);
        }

        public override void OnDragEnd(VisionViewBase view)
        {
            base.OnDragEnd(view);
            Sprite.SetCenterPosition(CurrentPosition.X, CurrentPosition.Y);
        }

        #endregion

    }
}

