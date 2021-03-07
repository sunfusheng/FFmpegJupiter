package com.sunfusheng.ffmpeg.jupiter

import android.content.Context
import android.text.TextUtils
import android.util.AttributeSet
import android.view.LayoutInflater
import android.widget.LinearLayout
import androidx.annotation.ColorRes
import com.sunfusheng.ffmpeg.jupiter.databinding.LayoutCommonItemViewBinding

/**
 * @author sunfusheng
 * @since  2021/02/26
 */
class CommonItemView @JvmOverloads constructor(
        context: Context,
        attrs: AttributeSet? = null,
        defStyleAttr: Int = 0
) : LinearLayout(context, attrs, defStyleAttr) {

    private val binding = LayoutCommonItemViewBinding.inflate(
            LayoutInflater.from(getContext()),
            this,
            true
    )

    fun setTitle(title: String) {
        binding.vTitle.text = title
    }

    fun setSubTitle(subtitle: String?) {
        binding.vSubtitle.visibility = if (TextUtils.isEmpty(subtitle)) GONE else VISIBLE
        binding.vSubtitle.text = subtitle
    }

    fun setStatus(status: String?) {
        binding.vStatus.visibility = if (TextUtils.isEmpty(status)) GONE else VISIBLE
        binding.vStatus.text = status
    }

    fun setStatusTextColor(@ColorRes colorId: Int) {
        binding.vStatus.setTextColor(resources.getColor(colorId))
    }

    fun setCommonInfo(title: String, subtitle: String? = "", status: String? = "") {
        setTitle(title)
        setSubTitle(subtitle)
        setStatus(status)
    }

    fun setDividerVisible(visible: Boolean) {
        binding.vDivider.visibility = if (visible) VISIBLE else INVISIBLE
    }
}