// Copyright (c) 2013 GitHub, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/native_window_win.h"

#include "base/values.h"
#include "common/options_switches.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/widget/widget.h"

namespace atom {

NativeWindowWin::NativeWindowWin(content::WebContents* web_contents,
                                 base::DictionaryValue* options)
    : NativeWindow(web_contents, options),
      window_(new views::Widget),
      web_view_(new views::WebView(NULL)) {
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
  window_->Init(params);

  int width = 800, height = 600;
  options->GetInteger(switches::kWidth, &width);
  options->GetInteger(switches::kHeight, &height);

  gfx::Size size(width, height);
  window_->CenterWindow(size);

  web_view_->SetWebContents(web_contents);
  window_->SetContentsView(web_view_);
}

NativeWindowWin::~NativeWindowWin() {
}

void NativeWindowWin::Close() {
  window_->Close();
}

void NativeWindowWin::CloseImmediately() {
  window_->CloseNow();
}

void NativeWindowWin::Move(const gfx::Rect& bounds) {
  window_->SetBounds(bounds);
}

void NativeWindowWin::Focus(bool focus) {
  if (focus)
    window_->Activate();
  else
    window_->Deactivate();
}

bool NativeWindowWin::IsFocused() {
  return window_->IsActive();
}

void NativeWindowWin::Show() {
  window_->Show();
}

void NativeWindowWin::Hide() {
  window_->Hide();
}

void NativeWindowWin::Maximize() {
  window_->Maximize();
}

void NativeWindowWin::Unmaximize() {
  window_->Restore();
}

void NativeWindowWin::Minimize() {
  window_->Minimize();
}

void NativeWindowWin::Restore() {
  window_->Restore();
}

void NativeWindowWin::SetFullscreen(bool fullscreen) {
  window_->SetFullscreen(fullscreen);
}

bool NativeWindowWin::IsFullscreen() {
  return window_->IsFullscreen();
}

void NativeWindowWin::SetSize(const gfx::Size& size) {
  window_->SetSize(size);
}

gfx::Size NativeWindowWin::GetSize() {
  return window_->GetWindowBoundsInScreen().size();
}

void NativeWindowWin::SetMinimumSize(const gfx::Size& size) {
  minimum_size_ = size;
}

gfx::Size NativeWindowWin::GetMinimumSize() {
  return minimum_size_;
}

void NativeWindowWin::SetMaximumSize(const gfx::Size& size) {
  maximum_size_ = size;
}

gfx::Size NativeWindowWin::GetMaximumSize() {
  return maximum_size_;
}

void NativeWindowWin::SetResizable(bool resizable) {
  resizable_ = resizable;

  // Show/Hide the maximize button.
  DWORD style = ::GetWindowLong(window_->GetNativeView(), GWL_STYLE);
  if (resizable)
    style |= WS_MAXIMIZEBOX;
  else
    style &= ~WS_MAXIMIZEBOX;
  ::SetWindowLong(window_->GetNativeView(), GWL_STYLE, style);
}

bool NativeWindowWin::IsResizable() {
  return resizable_;
}

void NativeWindowWin::SetAlwaysOnTop(bool top) {
  window_->SetAlwaysOnTop(top);
}

bool NativeWindowWin::IsAlwaysOnTop() {
  DWORD style = ::GetWindowLong(window_->GetNativeView(), GWL_EXSTYLE);
  return style & WS_EX_TOPMOST;
}

void NativeWindowWin::Center() {
  window_->CenterWindow(GetSize());
}

void NativeWindowWin::SetPosition(const gfx::Point& position) {
  window_->SetBounds(gfx::Rect(position, GetSize()));
}

gfx::Point NativeWindowWin::GetPosition() {
  return window_->GetWindowBoundsInScreen().origin();
}

void NativeWindowWin::SetTitle(const std::string& title) {
  title_ = title;
  window_->UpdateWindowTitle();
}

std::string NativeWindowWin::GetTitle() {
  return title_;
}

void NativeWindowWin::FlashFrame(bool flash) {
  window_->FlashFrame(flash);
}

void NativeWindowWin::SetKiosk(bool kiosk) {
  SetFullscreen(kiosk);
}

bool NativeWindowWin::IsKiosk() {
  return IsFullscreen();
}

gfx::NativeWindow NativeWindowWin::GetNativeWindow() {
  return window_->GetNativeView();
}

void NativeWindowWin::HandleKeyboardEvent(
    content::WebContents*,
    const content::NativeWebKeyboardEvent& event) {
  // Any unhandled keyboard/character messages should be defproced.
  // This allows stuff like F10, etc to work correctly.
  DefWindowProc(event.os_event.hwnd, event.os_event.message,
                event.os_event.wParam, event.os_event.lParam);
}

// static
NativeWindow* NativeWindow::Create(content::WebContents* web_contents,
                                   base::DictionaryValue* options) {
  return new NativeWindowWin(web_contents, options);
}

}  // namespace atom
