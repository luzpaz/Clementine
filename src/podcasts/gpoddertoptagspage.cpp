/* This file is part of Clementine.
   Copyright 2012, David Sansome <me@davidsansome.com>
   
   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gpoddertoptagsmodel.h"
#include "gpoddertoptagspage.h"
#include "core/closure.h"
#include "core/network.h"

#include <ApiRequest.h>

const int GPodderTopTagsPage::kMaxTagCount = 100;


GPodderTopTagsPage::GPodderTopTagsPage(Application* app, QWidget* parent)
  : AddPodcastPage(app, parent),
    network_(new NetworkAccessManager(this)),
    api_(new mygpo::ApiRequest(network_)),
    done_initial_load_(false)
{
  setWindowTitle(tr("Browse gpodder.net"));
  setWindowIcon(QIcon(":providers/mygpo32.png"));

  SetModel(new GPodderTopTagsModel(app, this));
}

GPodderTopTagsPage::~GPodderTopTagsPage() {
  delete api_;
}

void GPodderTopTagsPage::showEvent(QShowEvent* e) {
  QWidget::showEvent(e);

  if (!done_initial_load_) {
    // Start the request for list of top-level tags
    emit Busy(true);
    done_initial_load_ = true;

    mygpo::TagList* tag_list = api_->topTags(kMaxTagCount);
    NewClosure(tag_list, SIGNAL(finished()),
               this, SLOT(TagListLoaded(mygpo::TagList*)),
               tag_list);
  }
}

void GPodderTopTagsPage::TagListLoaded(mygpo::TagList* tag_list) {
  tag_list->deleteLater();

  emit Busy(false);

  foreach (mygpo::TagPtr tag, tag_list->list()) {
    model()->appendRow(model()->CreateFolder(tag->tag()));
  }
}
