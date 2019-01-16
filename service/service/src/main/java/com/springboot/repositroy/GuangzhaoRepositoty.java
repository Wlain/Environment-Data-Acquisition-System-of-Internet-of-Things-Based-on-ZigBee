package com.springboot.repositroy;

import com.springboot.entities.Guangzhao;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * Created by 10497 on 2017/6/12.
 */
@Repository
public interface GuangzhaoRepositoty extends JpaRepository<Guangzhao,Integer>{
    @Query("select gz,time from Guangzhao g ORDER BY  g.time desc ")
    List<Guangzhao> findAllGuangzhao();
}
