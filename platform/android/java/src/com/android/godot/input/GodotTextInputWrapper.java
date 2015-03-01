package com.android.godot.input;
import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import com.android.godot.*;

public class GodotTextInputWrapper implements TextWatcher, OnEditorActionListener {
	// ===========================================================
	// Constants
	// ===========================================================
	private static final String TAG = GodotTextInputWrapper.class.getSimpleName();

	// ===========================================================
	// Fields
	// ===========================================================
	private final GodotView mView;
	private final GodotEditText mEdit;
	private String mText;
	private String mOriginText;

	// ===========================================================
	// Constructors
	// ===========================================================

	public GodotTextInputWrapper(final GodotView view, final GodotEditText edit) {
		this.mView = view;
		this.mEdit = edit;
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	private boolean isFullScreenEdit() {
		final TextView textField = this.mEdit;
		final InputMethodManager imm = (InputMethodManager) textField.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		return imm.isFullscreenMode();
	}

	public void setOriginText(final String originText) {
		this.mOriginText = originText;
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void afterTextChanged(final Editable s) {
		if (this.isFullScreenEdit()) {
			return;
		}

		//if (BuildConfig.DEBUG) {
			//Log.d(TAG, "afterTextChanged: " + s);
		//}
		int nModified = s.length() - this.mText.length();
		if (nModified > 0) {
			final String insertText = s.subSequence(this.mText.length(), s.length()).toString();
			for(int i = 0; i < insertText.length(); i++) {
				int ch = insertText.codePointAt(i);
				mView.activity.mEngine.key(0, ch, true);
				mView.activity.mEngine.key(0, ch, false);
			}
			/*
			if (BuildConfig.DEBUG) {
				Log.d(TAG, "insertText(" + insertText + ")");
			}
			*/
		} else {
			for (; nModified < 0; ++nModified) {
				mView.activity.mEngine.key(KeyEvent.KEYCODE_DEL, 0, true);
				mView.activity.mEngine.key(KeyEvent.KEYCODE_DEL, 0, false);
				/*
				if (BuildConfig.DEBUG) {
					Log.d(TAG, "deleteBackward");
				}
				*/
			}
		}
		this.mText = s.toString();
	}

	@Override
	public void beforeTextChanged(final CharSequence pCharSequence, final int start, final int count, final int after) {
		/*
		if (BuildConfig.DEBUG) {
			Log.d(TAG, "beforeTextChanged(" + pCharSequence + ")start: " + start + ",count: " + count + ",after: " + after);
		}
		*/
		this.mText = pCharSequence.toString();
	}

	@Override
	public void onTextChanged(final CharSequence pCharSequence, final int start, final int before, final int count) {

	}

	@Override
	public boolean onEditorAction(final TextView pTextView, final int pActionID, final KeyEvent pKeyEvent) {
		if (this.mEdit == pTextView && this.isFullScreenEdit()) {
			// user press the action button, delete all old text and insert new text
			for (int i = this.mOriginText.length(); i > 0; i--) {
				mView.activity.mEngine.key(KeyEvent.KEYCODE_DEL, 0, true);
				mView.activity.mEngine.key(KeyEvent.KEYCODE_DEL, 0, false);
				/*
				if (BuildConfig.DEBUG) {
					Log.d(TAG, "deleteBackward");
				}
				*/
			}
			String text = pTextView.getText().toString();

			/* If user input nothing, translate "\n" to engine. */
			if (text.compareTo("") == 0) {
				text = "\n";
			}

			if ('\n' != text.charAt(text.length() - 1)) {
				text += '\n';
			}

			for(int i = 0; i < text.length(); i++) {
				int ch = text.codePointAt(i);
				mView.activity.mEngine.key(0, ch, true);
				mView.activity.mEngine.key(0, ch, false);
			}
			/*
			if (BuildConfig.DEBUG) {
				Log.d(TAG, "insertText(" + insertText + ")");
			}
			*/
		}
		
		if (pActionID == EditorInfo.IME_ACTION_DONE) {
			this.mView.requestFocus();
		}
		return false;
	}

	// ===========================================================
	// Methods
	// ===========================================================

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
