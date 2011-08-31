<?php
// $Id$
/**
 * Template to display project welcome page.
 *
 * Fields available:
 * $node: research project node object
 * $module_path: the path of module folder
 */
?>

<!-- start part of join link -->
<div class="join-link-div">
    <p><a href="<?=url("researchproject/termofuse/{$node->title}");?>">I want to join</a></p>
</div>
<!-- end part of join link -->

<!-- start part of Research tools -->
<div id="research-tools-div">
<h3>Research tools</h3>
<div class="research-tool-blocks">
    <?php foreach($node->tool_list as $tool): ?>
    <div class="research-tool-block">
        <div class="research-tool-log-div">
            <a class="tab-window-link" href=""><img class="research-tool-logo" src="<?=$tool['logo']?>" alt="<?=$tool['name']?>"/></a>
        </div>
        <p class="research-tool-name-p"><a class="tab-window-link" href="<?=url("research/tools/{$tool['id']}")?>"><?=$tool['name']?></a></p>
        <ul>
            <li><a class="tab-window-link" href="<?=url("research/tools/{$tool['id']}")?>">Read more</a></li>
            <li><a class="tab-window-link" href="<?=$tool['download']?>">View data</a></li>
        </ul>
    </div>
    <?php endforeach; ?>
</div><!-- the end of class research-tool-blocks -->
</div>
<!-- end part of Research tools -->

<!-- start part of Recent news -->
<div id="recent-news-div">
<h3>Recent News</h3>
<ul>
    <li><span class="news-title-span"><a href="">I am the title</a></span><span class="news-time-span">(31/08/2011)</span></li>
    <li><span class="news-title-span"><a href="">I am the title</a></span><span class="news-time-span">(31/08/2011)</span></li>
    <li><span class="news-title-span"><a href="">I am the title</a></span><span class="news-time-span">(31/08/2011)</span></li>
</ul>
</div>
<!-- end part of Recent news -->

<!-- start part of join link -->
<div class="join-link-div">
    <p><a href="<?=url("researchproject/termofuse/{$node->title}");?>">I want to join</a></p>
</div>
<!-- end part of join link -->
